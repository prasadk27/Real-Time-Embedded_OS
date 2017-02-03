import java.lang.Thread;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.locks.ReentrantReadWriteLock;

public class LockedHashMap {

//This is the shared object we are controlling access to
private Map theMap = new HashMap();
/*
  * This is the lock we are using to control shared access. Passing a value
* of true tells the ReadWriteLock to fairly allocate the locks.
  */
private ReentrantReadWriteLock theLock = new ReentrantReadWriteLock(true);

public static void main(String[] args) {
LockedHashMap lhm = new LockedHashMap();
lhm.testLocking();
}
public LockedHashMap() {
// Initialise the Map with some initial values
theMap.put("Key1", "Value1");
theMap.put("Key2", "Value2");
theMap.put("Key3", "Value3");
}

private void testLocking() {
/*
* These are the threads that will simultaniously attempt to access the shared object
*/
HashMapReader hmr1 = new HashMapReader("One", "Key1");
HashMapReader hmr2 = new HashMapReader("Two", "Key2");
HashMapReader hmr3 = new HashMapReader("Three", "Key3");
HashMapReader hmr4 = new HashMapReader("Four", "Key1");
HashMapReader hmr5 = new HashMapReader("Five", "Key2");
HashMapReader hmr6 = new HashMapReader("Six", "Key3");
HashMapReader hmr7 = new HashMapReader("Seven", "Key1");

HashMapWriter hmw1 = new HashMapWriter("One", "Key1", "Value_new1");
HashMapWriter hmw2 = new HashMapWriter("Two", "Key2", "Value_new2");
HashMapWriter hmw3 = new HashMapWriter("Three", "Key3", "Value_new3");

new Thread(hmr1).start();
new Thread(hmw1).start();
new Thread(hmr2).start();
new Thread(hmr3).start();
new Thread(hmw2).start();
new Thread(hmr4).start();
new Thread(hmw3).start();
new Thread(hmr5).start();
new Thread(hmr6).start();
new Thread(hmr7).start();

}

/**
*  A Reader takes the key of the object it is attempting to access,
*  gets the value, and prints the result. It Sleeps for 2 seconds while
*  holding the lock to simulate a more long running operation.
*/
private class HashMapReader implements Runnable {
private String name;
private String key;

public HashMapReader(String theName, String theKey) {
name = theName;
key = theKey;
}
public void run() {
try {
theLock.readLock().lock();
String value = theMap.get(key);
System.out.println("The Reader " + name + " has read the key " + key + " with a value " + value);
try {
Thread.sleep(2000);
} catch (InterruptedException e) {}
} finally {
theLock.readLock().unlock();
}
}

}

/**
*  A Writer takes ta key and a value, and updates the value of the key,
*  and then prints the result. It Sleeps for 4 seconds while
*  holding the lock to simulate a more long running operation.
*/
private class HashMapWriter implements Runnable {

private String name;
private String key;
private String value;

public HashMapWriter(String theName, String theKey, String theValue) {
name = theName;
key = theKey;
value = theValue;
}
public void run() {
try {
theLock.writeLock().lock();
theMap.put(key, value);
System.out.println("The Writer " + name + " has written the key " + key + " with the value "+ value);
try {
Thread.sleep(4000);
} catch (InterruptedException e) {
}
} finally {
theLock.writeLock().unlock();
}

}

}
}