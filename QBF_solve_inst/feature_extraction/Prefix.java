/* Prefix - Decompiled by JODE
 * Visit http://jode.sourceforge.net/
 */
import java.util.Iterator;
import java.util.Vector;

class Prefix
{
    private Vector prefix_ = new Vector();
    
    public Prefix() {
	/* empty */
    }
    
    public void add(PrefixSet prefixset) {
	prefix_.add(prefixset);
    }
    
    public int size() {
	return prefix_.size();
    }
    
    public Iterator itr() {
	return prefix_.iterator();
    }
    
    public void print() {
	Iterator iterator = itr();
	while (iterator.hasNext())
	    ((PrefixSet) iterator.next()).print();
    }
}
