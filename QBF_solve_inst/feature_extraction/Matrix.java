/* Matrix - Decompiled by JODE
 * Visit http://jode.sourceforge.net/
 */
import java.util.Iterator;
import java.util.Vector;

class Matrix
{
    private Vector matrix_ = new Vector();
    
    public Matrix() {
	/* empty */
    }
    
    public void add(Constraint constraint) {
	matrix_.add(constraint);
    }
    
    public int size() {
	return matrix_.size();
    }
    
    public Iterator itr() {
	return matrix_.iterator();
    }
    
    public void print() {
	Iterator iterator = itr();
	while (iterator.hasNext())
	    ((Constraint) iterator.next()).print();
    }
}
