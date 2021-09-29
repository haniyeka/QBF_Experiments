/* Constraint - Decompiled by JODE
 * Visit http://jode.sourceforge.net/
 */
import java.util.Iterator;
import java.util.Vector;

class Constraint
{
    private Vector constraint_ = new Vector();
    private int id_;
    
    public Constraint(int i) {
	id_ = i;
    }
    
    public int getId() {
	return id_;
    }
    
    public void add(int i) {
	constraint_.add(Integer.valueOf(i));
    }
    
    public int size() {
	return constraint_.size();
    }
    
    public Iterator itr() {
	return constraint_.iterator();
    }
    
    public void print() {
	Iterator iterator = itr();
	while (iterator.hasNext()) {
	    System.out.print(iterator.next());
	    System.out.print(" ");
	}
	System.out.println("");
    }
}
