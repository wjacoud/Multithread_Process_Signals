package ex03;


import java.util.Random;
import java.util.concurrent.Semaphore;

class MyThread extends Thread {
    Semaphore[] semaphores;
    Account[] accounts;
    String threadName;

    public MyThread(Semaphore[] semaphores, Account[] accounts, String threadName) {
        super(threadName);
        this.semaphores = semaphores;
        this.accounts = accounts;
        this.threadName = threadName;

    }

    @Override
    public void run() {

        System.out.println("Cria " + threadName);
        try {
            Random random = new Random();
            int rdn1 = random.ints(0,9).findFirst().getAsInt();
            int rdn2;
            do {
                rdn2 = random.ints(0,9).findFirst().getAsInt();
            } while (rdn1 == rdn2);
            Double tmpAmount = random.doubles(10.0, 200.0).findFirst().getAsDouble();
            System.out.printf("Transferencia solicitada da conta %d para a conta %d no valor de: %.2f\n", rdn1, rdn2, tmpAmount);
            synchronized (accounts){
                transferMoney(accounts[rdn1],accounts[rdn2], tmpAmount);
            }
            Thread.sleep(100);

        } catch (InterruptedException exc) {
            System.out.println(exc);
        }
    }

    public boolean transferMoney(Account fromAcct,Account toAcct, Double amount) throws InterruptedException {
        semaphores[fromAcct.getNumber()].acquire();
        semaphores[toAcct.getNumber()].acquire();
        accounts[fromAcct.getNumber()].withdraw(amount);
        accounts[toAcct.getNumber()].deposit(amount);
        System.out.println("Transferencia executada");
        System.out.printf("Conta de Origem: %.2f\n", accounts[fromAcct.getNumber()].getBalance());
        System.out.printf("Conta de destino: %.2f:\n", accounts[toAcct.getNumber()].getBalance());
        semaphores[fromAcct.getNumber()].release();
        semaphores[toAcct.getNumber()].release();
        return true;
    }

}

//Classe Tickets onde fica a zona de memória partilhada
class Account {
    private int number;
    private Double balance;

    public Account(int number, Double balance) {
        this.number = number;
        this.balance = balance;
    }

    public int getNumber() {
        return number;
    }

    public Double getBalance() {
        return balance;
    }

    public void deposit(Double amount) {
        this.balance += amount;
    }

    public void withdraw(Double amount) {
        this.balance -= amount;
    }
}

class MySemaphore {
    public static void main(String args[]) throws InterruptedException {


        //Array de Threads
        Thread[] takersThreads = new Thread[10];
        Semaphore[] semaphores = new Semaphore[10];
        Account[] accounts = new Account[10];
        for (int i = 0; i < 10; i++) {
            semaphores[i] = new Semaphore(1);
            accounts[i] = new Account(i, 1000.00);
        }
        for (int i = 0; i < takersThreads.length; i++) {
            takersThreads[i] = new MyThread(semaphores, accounts, "Caixa - " + i);
            takersThreads[i].start();
        }

        // A espera das Threads
        for (int i = 0; i < takersThreads.length; i++) {
            takersThreads[i].join();
        }
        for (Account acc: accounts) {
            System.out.printf("Saldo final da conta - %d : %.2f\n", acc.getNumber(), acc.getBalance());
        }
    }
}


