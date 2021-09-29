/* QbfVariable - Decompiled by JODE
 * Visit http://jode.sourceforge.net/
 */
import java.util.Iterator;
import java.util.Vector;

class QbfVariable
{
    private int id_;
    private int level_;
    private char q_;
    private Vector posOccs_ = new Vector();
    private Vector negOccs_ = new Vector();
    
    public QbfVariable(int i, int i_0_, char c) {
	id_ = i;
	level_ = i_0_;
	q_ = c;
    }
    
    public int getLevel() {
	return level_;
    }
    
    public void setLevel(int i) {
	level_ = i;
    }
    
    public boolean isExist() {
	return q_ == 'e';
    }
    
    public boolean isForall() {
	return q_ == 'a';
    }
    
    public int getId() {
	return id_;
    }
    
    public void setId(int i) {
	id_ = i;
    }
    
    public void addPosOcc(int i) {
	posOccs_.add(Integer.valueOf(i));
    }
    
    public void addNegOcc(int i) {
	negOccs_.add(Integer.valueOf(i));
    }
    
    public Iterator posOccsItr() {
	return posOccs_.iterator();
    }
    
    public Iterator negOccsItr() {
	return negOccs_.iterator();
    }
    
    public int posOccs() {
	return posOccs_.size();
    }
    
    public int negOccs() {
	return negOccs_.size();
    }
    
    public void printPosOccs() {
	Iterator iterator = posOccsItr();
	while (iterator.hasNext()) {
	    System.out.print(iterator.next());
	    System.out.print(" ");
	}
	System.out.println("");
    }
    
    public void printNegOccs() {
	Iterator iterator = negOccsItr();
	while (iterator.hasNext()) {
	    System.out.print(iterator.next());
	    System.out.print(" ");
	}
	System.out.println("");
    }
}
