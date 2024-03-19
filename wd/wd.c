#define _POSIX_C_SOURCE 200112L/* getenv */
#include <assert.h>
#include <signal.h> /* sigaction */
#include <stdio.h> /* perror */
#include <stdlib.h> /* getenv, putenv, setenv */
#include <string.h> /* strcmp, memset */
#include <sys/types.h> /* pid_t */
#include <unistd.h> /* fork */
#include <pthread.h> /* pthtad_create */
#include <sys/types.h> /*pid_t*/
#include <sys/wait.h> /*waitpid*/

#include "wd.h"
#include "pqueue.h"
#include "uid.h"
#include "scheduler.h"
#include "task.h"

#define UNUSED(a) ((void)(a))

typedef enum 
{
	WD = 0, 
	APP = 1
} process_e;

typedef enum 
{
	NOT_RECEIVE = 0, 
	RECEIVE = 1
} status_receive;

typedef struct process_params_s
{
	process_e process_name;
	pthread_t *thread_app;
	pid_t pid_to_signal;
	pid_t process_pid;
	char **argv;
} proc_params_t;

const int FAILED = -1;

typedef status_t (*exec_func)(proc_params_t);

status_t AllocMemory(proc_params_t *app_params, void **params);
status_t AppCreateThread(proc_params_t process_params);
status_t ExecProc(proc_params_t process_params);
status_t InitParamStartWd(proc_params_t *app_params, proc_params_t *wd_params, 
													char **argv, void **params);
status_t Restart(proc_params_t process_params);
status_t RestartProc(proc_params_t process_params, exec_func func);
status_t RunWDProc(proc_params_t process_params);
int RestartProcess(proc_params_t process_params);
int SendAndReciveSignal(proc_params_t process_params);
int UnsetEnv(proc_params_t process_params);
task_status_t CheckSignal(void *scheduler);
task_status_t SendSignal(void *process_params);
void *WrapperSendAndReciveSignal(void *process_params);
void InitProcParams(proc_params_t *process_params, process_e identity, 
									pid_t process_id, pid_t to_signal);
void InitSigAction();
void InitSigActionStop();
void SigHand(int sig);
void SigHandStop(int sig);

int flag_send_signal = NOT_RECEIVE; 
int flag_stop = NOT_RECEIVE;

status_wd StartWd(int argc, char const *argv[], void **params)
{
	char *wd_env = "wd", *app_env = "app";
	char *is_wd_alive = NULL, *is_app_alive = NULL;
	proc_params_t wd_params = {0}, app_params = {0};
	pid_t wd_pid = {0};

	UNUSED(argc);

	if (FAIL == AllocMemory(&app_params, params))
	{
		return (WD_ALLOC_FAILED);
	}

	InitSigAction();

	wd_params.argv = (char **)argv;
	app_params.argv = (char **)argv;

   is_wd_alive = getenv(wd_env);
    is_app_alive = getenv(app_env);

	if (NULL != is_wd_alive && NULL != is_app_alive)
	{
		return (WD_SUCCESS);
	}

	if (SUCCESS != setenv(app_env, "ALIVE", 1))
	{
		perror("setenv fail ");
		return (WD_FAILED);
	}

	if (NULL == is_wd_alive) 
	{
		if (SUCCESS != setenv(wd_env, "ALIVE", 1)) 
		{
			perror("setenv fail ");
			return (WD_FAILED);
		}

		wd_pid = fork();
		if (0 == wd_pid) 
		{
			InitProcParams(&wd_params, WD, getpid(), getppid());
			if (FAIL == SendAndReciveSignal(wd_params))
			{
				return (WD_FAILED);
			}
		}
		else if (wd_pid > 0) 
		{
			InitProcParams(&app_params, APP, getpid(), wd_pid);
			AppCreateThread(app_params);
		}
		else if (wd_pid < 0)
		{
			perror("fork FAIL ");
			return (WD_FAILED);
		}
	}
	else   /* --> after the first time */
	{
		InitProcParams(&app_params, APP, getpid(), getppid());
		AppCreateThread(app_params);
	}

	memmove(*params, &wd_params, sizeof(void *));

	return (WD_SUCCESS);
}

status_t AllocMemory(proc_params_t *app_params, void **params)
{
	app_params->thread_app = malloc(sizeof(app_params->thread_app));
	*params = malloc(sizeof(*params));
	if (NULL == app_params->thread_app || NULL == *params)
	{
		return (FAIL);
	}

	return (SUCCESS);
}

void InitProcParams(proc_params_t *process_params, process_e identity, 
										pid_t process_id, pid_t to_signal)
{
	process_params->process_name = identity;
	process_params->process_pid = process_id;
	process_params->pid_to_signal = to_signal;
}

void SigHand(int sig)
{
	UNUSED(sig);
	
	flag_send_signal = RECEIVE;
}

void InitSigAction()
{
	struct sigaction sa_usr1;
    memset (&sa_usr1, 0, sizeof (sa_usr1)); 

    sa_usr1.sa_flags = SA_SIGINFO;
    sa_usr1.sa_handler = SigHand;

    assert (FAILED != sigaction(SIGUSR1, &sa_usr1, NULL));
}	

void *WrapperSendAndReciveSignal(void *process_params)
{
	proc_params_t app_params = {0};
	
	assert(NULL != process_params);

	memmove(&app_params, process_params, sizeof(proc_params_t));
	
	free(process_params);
	process_params = NULL;
	
	if (FAIL == SendAndReciveSignal(app_params))
	{
		return (NULL);
	}

	return (process_params);
}

int SendAndReciveSignal(proc_params_t process_params)
{
	task_uid_t check_signal = {0};
	task_uid_t send_signal = {0};
	scheduler_t *scheduler = NULL;
	scheduler_status_t run_ret = 0;
	
	scheduler = TSCreate();
	if (NULL == scheduler)
	{
		printf("schedular FAIL %d\n", __LINE__);
		return (FAIL);
	}

	check_signal = TSAddTask(scheduler, 3, CheckSignal, scheduler);
	send_signal = TSAddTask(scheduler, 1, SendSignal, &process_params);
	if (UIDIsSame(BAD_UID, check_signal) || UIDIsSame(BAD_UID, send_signal))
	{
		printf("TSAddTask FAIL, %d\n", __LINE__);
		return (FAIL);
	}

	run_ret = TSRun(scheduler);
	if (FAILED_SCHEDULER == run_ret)
	{
		printf("run-schedular FAIL %d\n", __LINE__);
		return (FAIL);
	}
	
	if (STOPPED_SCHEDULER == run_ret)
	{
		TSDestroy(scheduler);	

		if (RECEIVE == flag_stop)
		{
			if (APP == process_params.process_name)
			{
				assert(SUCCESS == pthread_join(*process_params.thread_app, NULL));
			}
			
			return (SUCCESS);
		}

		if (FAIL == UnsetEnv(process_params))
		{
			return (FAIL);
		}
		
		if(FAIL == Restart(process_params)) 
		{
			return (FAIL);
		}
	}

	return (SUCCESS);
}

int UnsetEnv(proc_params_t process_params)
{
	if (WD == process_params.process_name)
	{
		puts("unset to: app, app is dead\n");
		if (FAILED == unsetenv("app"))
		{
			printf("unsetenv FAIL %d\n", __LINE__);
			perror(" ");
			
			return (FAIL);
		}	
	}
	else
	{
		puts("wd is dead\n");
	}

	return (SUCCESS);
}

status_t AppCreateThread(proc_params_t process_params)
{
	proc_params_t *app_params = (proc_params_t *)malloc(sizeof(proc_params_t));
	if (NULL == app_params)
	{
		return (FAIL);
	}

	memmove(app_params, &process_params, sizeof(proc_params_t));
	if(SUCCESS != pthread_create (process_params.thread_app, NULL, 
							WrapperSendAndReciveSignal, app_params))
	{
		free(app_params);
		app_params = NULL;
		
		return (FAIL);
	}

	return (SUCCESS);
}

task_status_t CheckSignal(void *scheduler)
{
	assert(NULL != scheduler);

	if (RECEIVE == flag_stop)
	{
		TSStop((scheduler_t *)scheduler);

		return (DONT_RESCHEDULE);
	}

	if (NOT_RECEIVE == flag_send_signal)
	{
		TSStop((scheduler_t *)scheduler);

		return (DONT_RESCHEDULE);
	}
	
	flag_send_signal = NOT_RECEIVE;

	return (RESCHEDULE);
}

task_status_t SendSignal(void *params)
{
	proc_params_t *params_process = (proc_params_t *)params;

	assert(NULL != params);

	kill(params_process->pid_to_signal, SIGUSR1);

	return (RESCHEDULE);
}

status_t ExecProc(proc_params_t process_params)
{
	if (SUCCESS != execvp(process_params.argv[0], process_params.argv))
	{
		return (FAIL);
	}

	return (SUCCESS);
}

status_t RunWDProc(proc_params_t process_params)
{
	InitProcParams(&process_params, WD, getpid(), getppid());

	if (FAIL == SendAndReciveSignal(process_params))
	{
		return (FAIL);
	}

	return (SUCCESS);
}

status_t RestartProc(proc_params_t process_params, exec_func func)
{
	pid_t pid = fork();
	
	if (0 == pid)
	{
		func(process_params);
	}
	else if (pid > 0)
	{
		process_params.pid_to_signal = pid;
		
		if (FAIL == SendAndReciveSignal(process_params))
		{
			return (FAIL);
		}
	} 
	else 
	{
		perror("fork FAIL");
		
		return (FAIL);
	}			

	return (SUCCESS);
}

status_t Restart(proc_params_t process_params)
{
	status_t exit_status = SUCCESS;

	puts("Restart\n");

	if (process_params.process_name == WD) /* wd aive and benni is dead */
	{
		exit_status = RestartProc(process_params, ExecProc);
	}
	else /* benni alive and wd is dead */
	{
		exit_status = RestartProc(process_params, RunWDProc);
	}

	return (exit_status);
}

void SendStop(proc_params_t proc_params)
{
	kill(proc_params.pid_to_signal, SIGUSR2); 
	kill(proc_params.process_pid, SIGUSR2); 
}

void SigHandStop(int sig)
{
	UNUSED(sig);
	
	flag_stop = RECEIVE;
}

void InitSigActionStop()
{
	struct sigaction sa;
    memset (&sa, 0, sizeof (sa)); 

    sa.sa_flags = SA_SIGINFO;
    sa.sa_handler = SigHandStop;

   	assert (FAILED != sigaction(SIGUSR2, &sa, NULL));
}	

void StopWd(void *params)
{
	proc_params_t proc_params = {0};
	memmove(&proc_params, params, sizeof(proc_params_t));
	
	InitSigActionStop();

	SendStop(proc_params);

	if (getppid() != proc_params.pid_to_signal) /* app is the child */
	{
		waitpid(proc_params.pid_to_signal, NULL, WUNTRACED);
	}

	free(params);
	params = NULL;
}
