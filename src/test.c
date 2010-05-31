struct job_t {
	void *(*job)(void *);
	void *arg;
	struct job_t *next;
};

typedef struct {
	int count;
	int nqueue;
	pthread_t *threads;
	struct job_t *head, tail;
	pthread_mutex_t lock;
	pthread_cond_t 
} thr_pool_t;

