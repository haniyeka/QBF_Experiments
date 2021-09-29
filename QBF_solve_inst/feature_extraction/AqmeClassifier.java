/* AqmeClassifier - Decompiled by JODE
 * Visit http://jode.sourceforge.net/
 */
import java.io.BufferedReader;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.PrintStream;

import weka.core.Instance;
import weka.core.Instances;

public class AqmeClassifier
{
    protected Instances trainingSet_;
    protected Instances testSet_;
    
    public void printTrainingSet() {
	System.out.println(trainingSet_);
    }
    
    public void printTestInstance() {
	System.out.println(testSet_.instance(0));
    }
    
    protected void setTrainingSet_(String string) throws Exception {
	trainingSet_
	    = new Instances(new BufferedReader(new FileReader(string)));
	trainingSet_.setClassIndex(trainingSet_.numAttributes() - 1);
	testSet_
	    = new Instances(new BufferedReader(new FileReader(string)), 0);
	testSet_.setClassIndex(testSet_.numAttributes() - 1);
    }
    
    protected Instances getTrainingSet_() {
	return trainingSet_;
    }
    
    protected void setTestInstance_(double[] ds) {
	Instance instance = new Instance(testSet_.numAttributes());
	for (int i = 0; i < ds.length; i++)
	    instance.setValue(i, ds[i]);
	testSet_.add(instance);
    }
    
    protected Instance getTestInstance_() {
	return testSet_.instance(0);
    }
    
    protected void updateTrainingSet_(String string, String string_0_)
	throws Exception {
	testSet_.instance(0).setClassValue(string_0_);
	trainingSet_.add(testSet_.instance(0));
	FileOutputStream fileoutputstream = new FileOutputStream(string);
	new PrintStream(fileoutputstream).println(trainingSet_.toString());
	fileoutputstream.close();
    }
}
