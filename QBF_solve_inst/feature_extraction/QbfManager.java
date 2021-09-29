/* QbfManager - Decompiled by JODE
 * Visit http://jode.sourceforge.net/
 */

class QbfManager
{
    private String qdimacsFormula_;
    private QbfFormula formula_ = new QbfFormula();
    
    public QbfManager(String string) {
	qdimacsFormula_ = string;
    }
    
    public void loadData() {
	QbfParser qbfparser = new QbfParser(qdimacsFormula_, formula_);
	qbfparser.parse();
    }
    
    public double[] extractFeatures() {
	QbfFeatureManager qbffeaturemanager = new QbfFeatureManager(formula_);
	qbffeaturemanager.computeFeatures();
	return qbffeaturemanager.getFeatures();
    }

    public void print() {
	QbfFeatureManager qbffeaturemanager = new QbfFeatureManager(formula_);
	qbffeaturemanager.computeFeatures();
	qbffeaturemanager.print();
    }
}
