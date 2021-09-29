/* ClassifierManager - Decompiled by JODE
 * Visit http://jode.sourceforge.net/
 */

class ClassifierManager
{
    private String formulaName_;
    private String arffFileName_;
    private String modelFileName_;
    private String classifier_;
    private String predictedSolver_;
    private AqmeClassifier1NN nn_;
    private AqmeClassifierC4_5 c45_;
    private AqmeClassifierRipper ripper_;
    private AqmeClassifierMlr mlr_;
    private double[] features_;
    
    public ClassifierManager(String string, String string_0_, String string_1_,
			     String string_2_, double[] ds) throws Exception {
	nn_ = new AqmeClassifier1NN();
	c45_ = new AqmeClassifierC4_5();
	ripper_ = new AqmeClassifierRipper();
	mlr_ = new AqmeClassifierMlr();
	formulaName_ = string;
	arffFileName_ = string_0_;
	modelFileName_ = string_1_;
	classifier_ = string_2_;
	features_ = ds;
	if (classifier_.equals("1NN"))
	    predictedSolver_
		= nn_.predict(arffFileName_, modelFileName_, features_);
	else if (classifier_.equals("C4.5"))
	    predictedSolver_
		= c45_.predict(arffFileName_, modelFileName_, features_);
	else if (classifier_.equals("RIPPER"))
	    predictedSolver_
		= ripper_.predict(arffFileName_, modelFileName_, features_);
	else if (classifier_.equals("MLR"))
	    predictedSolver_
		= mlr_.predict(arffFileName_, modelFileName_, features_);
    }
    
    public String getPredictedSolver() {
	return predictedSolver_;
    }
    
    public void retrain(String string) throws Exception {
	if (classifier_.equals("1NN"))
	    nn_.retrain(arffFileName_, modelFileName_, string);
	else if (classifier_.equals("C4.5"))
	    c45_.retrain(arffFileName_, modelFileName_, string);
	else if (classifier_.equals("RIPPER"))
	    ripper_.retrain(arffFileName_, modelFileName_, string);
	else if (classifier_.equals("MLR"))
	    mlr_.retrain(arffFileName_, modelFileName_, string);
    }
}
