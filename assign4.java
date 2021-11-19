import java.util.*;
import java.util.Scanner;
import java.util.concurrent.TimeUnit;

public class assign4 {
    public static void main(String args[]){
        if(args.length < 1){
            System.out.println("ERROR! Not enough command line args!");
            return;
        }

        int n = Integer.parseInt(args[0]);
        double[] A = new double[n];

        for(int i = 0 ; i < n; i++){
            A[i] = (double)(Math.random() * 100) + 1;
        }
        double[] B = A, C = new double[n], Afirst = new double[n/2], Asecond = new double[n/2];
        for(int i = 0; i < n/2; i++){
            Afirst[i] = A[i];
        }
        int count = 0;
        for(int i = n/2; i < n; i++){
            if(count < n/2) {
                Asecond[count] = A[i];
            }
            count++;
        }

        // Thread 1 sort A into B -------------------------------------------------------------------
        selectionsort th1 = new selectionsort(B, n);
        double start = System.nanoTime();
        th1.start();
        try{
            th1.join();
        }catch (InterruptedException e){}
        //calc end time of thread
        B = th1.getArr();
        double end = System.nanoTime();
        double time = (end - start) / 1000000000.0;
        System.out.printf("Sorting by ONE thread is done in %f ms\n", time*1000);

        // Thread 2 sort first half ---------------------------------------------------------------
        selectionsort th2 = new selectionsort(Afirst, n/2);
        start = System.nanoTime();
        th2.start();
        try{
            th2.join();
        }catch(InterruptedException e) {}
        Afirst = th2.getArr();

        //thread 3 sort second half --------------------------------------------------------------
        selectionsort th3 = new selectionsort(Asecond, n/2);
        th3.start();
        try{
            th3.join();
        }catch(InterruptedException e) {}
        Asecond = th3.getArr();
        // Thread 4 merge first and second half --------------------------------------------------
        merge th4 = new merge(Afirst, Asecond, C, n/2, n);
        C = th4.getRet();
        th4.start();
        try {
            th4.join();
        }catch (InterruptedException e){}
        C = th4.getRet();
        //calc end time of threads
        end = System.nanoTime();
        time = (end - start) / 1000000000.0;
        System.out.printf("Sorting by TWO threads is done in %f ms\n", time*1000);
    }
}

class selectionsort extends Thread{
    private double[] arr;
    private int size;

    public selectionsort(double[] arr, int size){
        this.arr = arr;
        this.size = size;
    }
    public double[] getArr(){
        return arr;
    }
    public void run(){
        //write selectionsort algo
        for(int i = 0; i < size - 1; i++){
            int minIndx = i;
            for(int j = i + 1; j < size; j++){
                if(arr[j] < arr[minIndx]){
                    minIndx = j;
                }
                double temp = arr[minIndx];
                arr[minIndx] = arr[i];
                arr[i] = temp;
            }
        }
    }
}

class merge extends Thread{
    private double[] arr1, arr2, ret;
    private int halfSize, size;

    public merge(double[] arr1, double[] arr2, double[] ret, int halfSize, int size) {
        this.arr1 = arr1;
        this.arr2 = arr2;
        this.ret = ret;
        this.halfSize = halfSize;
        this.size = size;
    }
    public double[] getRet(){
        return ret;
    }
    public void run(){
        //merge the 2 arrs
        int i = 0, j = 0, k=0;
        while(i < halfSize && j < halfSize){
            if(arr1[i] <= arr2[j]){
                ret[k++] = arr1[i++];
            }
            else {
                ret[k++] = arr2[j++];
            }
        }

        while(i < halfSize){
            ret[k++] = arr1[i++];
        }
        while(j < halfSize){
            ret[k++] = arr2[j++];
        }
    }
}
