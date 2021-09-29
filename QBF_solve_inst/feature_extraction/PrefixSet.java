/* PrefixSet - Decompiled by JODE
 * Visit http://jode.sourceforge.net/
 */
import java.util.Iterator;
import java.util.Vector;

class PrefixSet
{
    private Vector set_ = new Vector();
    private char quantifier_ = 'e';
    
    public PrefixSet() {
	/* empty */
    }
    
    public void setExist() {
	quantifier_ = 'e';
    }
    
    public void setForall() {
	quantifier_ = 'a';
    }
    
    public boolean isExist() {
	return quantifier_ == 'e';
    }
    
    public boolean isForall() {
	return quantifier_ == 'a';
    }
    
    public void add(int i) {
	set_.add(Integer.valueOf(i));
    }
    
    public int size() {
	return set_.size();
    }
    
    public Iterator itr() {
	return set_.iterator();
    }
    
    public void print() {
	System.out.print(quantifier_);
	Iterator iterator = itr();
	while (iterator.hasNext())
	    System.out.print(new StringBuilder().append(" ").append
				 (((Integer) iterator.next()).intValue())
				 .toString());
	System.out.println("");
    }
}
