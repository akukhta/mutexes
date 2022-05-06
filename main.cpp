#include <iostream>
#include <fstream>
#include <unistd.h>
#include <vector>

enum class sType : char { mutex, spin, rwr, rww};

struct syncType
{
    sType type;
};

static pthread_mutex_t m;
static pthread_spinlock_t lock;
static pthread_rwlock_t rwlock;

void* thread_mutex_func(void *data)
{
    syncType *s = reinterpret_cast<syncType*>(data);

    switch (s->type)
      {
        case sType::mutex:
          {
              pthread_mutex_lock(&m);
              break;
          };

        case sType::spin:
          {
              pthread_spin_lock(&lock);
              break;
          };

        case sType::rwr:
          {
              pthread_rwlock_rdlock(&rwlock);
              break;
          };

        case sType::rww:
          {
              pthread_rwlock_wrlock(&rwlock);
              break;
          };

        default:
          {
              break;
          }
      }
  return nullptr;
}

pthread_t createThread(syncType &sT)
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_t pId;
    pthread_create(&pId, &attr, thread_mutex_func, reinterpret_cast<void*>(&sT));
    return pId;
}

int main() {
    std::vector<pthread_t> pids;
    std::ofstream out("/home/box/main.pid");
    out << getpid();
    out.close();
    std::cout << getpid() << std::endl;

    //Mutex
    m = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&m);

    syncType mT;
    mT.type = sType::mutex;
    pids.push_back(createThread(mT));
    //

    //spinlock
    pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE);
    pthread_spin_lock(&lock);

    syncType sT;
    sT.type = sType::spin;
    pids.push_back(createThread(sT));
    //

    //rwlock
    rwlock = PTHREAD_RWLOCK_INITIALIZER;
    pthread_rwlock_wrlock(&rwlock);
    syncType rwT;

    rwT.type = sType::rwr;
    pids.push_back(createThread(rwT));
    //

    rwT.type = sType::rww;
    pids.push_back(createThread(rwT));
    
    for (auto pid : pids)
    {
        pthread_join(pid, NULL);    
    }
    
    return 0;
} 
