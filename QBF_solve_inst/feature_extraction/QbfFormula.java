/* QbfFormula - Decompiled by JODE
 * Visit http://jode.sourceforge.net/
 */
import java.util.Vector;

class QbfFormula
{
    private Prefix prefix_;
    private Matrix matrix_;
    private Vector variable_;
    
    public QbfFormula() {
	prefix_ = new Prefix();
	matrix_ = new Matrix();
	variable_ = new Vector();
    }
    
    public QbfFormula(Prefix prefix, Matrix matrix) {
	prefix_ = new Prefix();
	matrix_ = new Matrix();
	variable_ = new Vector();
	prefix_ = prefix;
	matrix_ = matrix;
    }
    
    public void setNvars(int i) {
	variable_.setSize(i + 1);
    }
    
    public Prefix getPrefix() {
	return prefix_;
    }
    
    public void setPrefix(Prefix prefix) {
	prefix_ = prefix;
    }
    
    public Matrix getMatrix() {
	return matrix_;
    }
    
    public void setMatrix(Matrix matrix) {
	matrix_ = matrix;
    }
    
    public Vector getVariables() {
	return variable_;
    }
    
    public void setVariable(int i, QbfVariable qbfvariable) {
	variable_.set(i, qbfvariable);
    }
    
    public QbfVariable getVariable(int i) {
	return (QbfVariable) variable_.get(i);
    }
    
    public void print() {
	prefix_.print();
	matrix_.print();
    }
}
