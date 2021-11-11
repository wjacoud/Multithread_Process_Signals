package ex02;

//Classe Tickets onde fica a zona de memória partilhada
class Tickets
{
    static int count = 20;
}

class MyThread extends Thread
{
    java.util.concurrent.Semaphore sem;
    String threadName;
    public MyThread(java.util.concurrent.Semaphore sem, String threadName)
    {
        super(threadName);
        this.sem = sem;
        this.threadName = threadName;
    }

    @Override
    public void run() {

            System.out.println("Cria " + threadName);
            try
            {
                System.out.println(threadName + " a espera de permissão");

                // Adquire a chave
                sem.acquire();

                System.out.println(threadName + " vende Ticket");
                    Tickets.count--;
                    System.out.println(threadName + ": " + Tickets.count);

                    Thread.sleep(100);

            } catch (InterruptedException exc) {
                System.out.println(exc);
            }
            // Liberta a chave
            System.out.println(threadName + " liberta o acesso.");
            sem.release();
        }

}

class Semaphore
{
    public static void main(String args[]) throws InterruptedException
    {
        java.util.concurrent.Semaphore sem = new java.util.concurrent.Semaphore(1);

        //Array de Threads
        Thread[] takersThreads = new Thread[20];

        for (int i = 0; i < takersThreads.length; i++) {
            takersThreads[i] = new MyThread(sem, "Vendedor - " + i);
            takersThreads[i].start();
        }

        // A espera das Threads
        for (int i = 0; i < takersThreads.length; i++) {
            takersThreads[i].join();
        }

        System.out.println("count: " + Tickets.count);
    }
}
