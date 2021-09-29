/* Strategy - Decompiled by JODE
 * Visit http://jode.sourceforge.net/
 */
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

class Strategy
{
    private String[] rawSolverOrder_ = new String[5];
    private String[] yasmSolverOrder_ = new String[5];
    private String[] hybridSolverOrder_ = new String[2];
    private String[] searchSolverOrder_ = new String[3];
    private Map solversMap_ = new HashMap();
    private Map solversMessages_ = new HashMap();
    private String[] rndSolverOrder_ = new String[5];
    private String formulaName_;
    private String solversPath_;
    private String timeLimit_;
    private String memoryLimit_;
    private String strategy_;
    private double[] features_;
    private ClassifierManager cManager_;
    
    public Strategy(String string, String string_0_, String string_1_,
		    String string_2_, String string_3_, String string_4_,
		    String string_5_, String string_6_,
		    double[] ds) throws Exception {
	solversPath_ = string;
	timeLimit_ = string_0_;
	memoryLimit_ = string_1_;
	strategy_ = string_2_;
	formulaName_ = string_3_;
	features_ = ds;
	cManager_ = new ClassifierManager(formulaName_, string_4_, string_5_,
					  string_6_, ds);
	solversMessages_.put
	    ("2clsQ",
	     "Running 2clsQ (Copyright by H. Samulowitz and F. Bacchus)");
	solversMessages_.put
	    ("sKizzo",
	     "Running sKizzo 0.8.2, (Copyright by M. Benedetti, http://skizzo.info)");
	solversMessages_.put
	    ("QuBE",
	     "Running QuBE 3.0, (Copyright by E. Giunchiglia, M. Narizzano, and A. Tacchella)");
	solversMessages_.put
	    ("sSolve",
	     "Running sSolve, (Copyright by R. Feldmann and S. Schamberger)");
	solversMessages_.put("quantor",
			     "Running quantor, (Copyright by A. Biere)");
    }
    
    public int run() throws Exception {
	if (strategy_.equals("PLAIN"))
	    return plainStrategy_();
	String[] strings = strategy_.split("_");
	String string = strings[0];
	String string_7_ = strings[1];
	initialize_();
	if (string_7_.equals("TPE"))
	    return wpeStrategy_(string);
	if (string_7_.equals("AES"))
	    return edtStrategy_(string);
	if (string_7_.equals("ITR"))
	    return omrrStrategy_(string);
	System.out.println("ERROR");
	return 0;
    }
    
    private void initialize_() {
	rawSolverOrder_[0] = "sKizzo";
	rawSolverOrder_[1] = "QuBE";
	rawSolverOrder_[2] = "quantor";
	rawSolverOrder_[3] = "sSolve";
	rawSolverOrder_[4] = "2clsQ";
	yasmSolverOrder_[0] = "sKizzo";
	yasmSolverOrder_[1] = "quantor";
	yasmSolverOrder_[2] = "QuBE";
	yasmSolverOrder_[3] = "sSolve";
	yasmSolverOrder_[4] = "2clsQ";
	solversMap_.put("2clsQ", "SEARCH");
	solversMap_.put("sKizzo", "HYBRID");
	solversMap_.put("quantor", "HYBRID");
	solversMap_.put("QuBE", "SEARCH");
	solversMap_.put("sSolve", "SEARCH");
	hybridSolverOrder_[0] = "sKizzo";
	hybridSolverOrder_[1] = "quantor";
	searchSolverOrder_[0] = "QuBE";
	searchSolverOrder_[1] = "sSolve";
	searchSolverOrder_[2] = "2clsQ";
	rndSolverOrder_[0] = "sKizzo";
	rndSolverOrder_[1] = "quantor";
	rndSolverOrder_[2] = "QuBE";
	rndSolverOrder_[3] = "sSolve";
	rndSolverOrder_[4] = "2clsQ";
	Collections.shuffle(Arrays.asList(rndSolverOrder_));
    }
    
    private int runSolver_(String string, int i) throws Exception {
	Runtime runtime = Runtime.getRuntime();
	String[] strings = new String[5];
	strings[0] = "./launcher.pl";
	strings[1] = new StringBuilder().append(solversPath_).append
			 (string).toString();
	strings[2] = formulaName_;
	strings[3] = Integer.toString(i);
	strings[4] = memoryLimit_;
	System.out.println(new StringBuilder().append
			       (solversMessages_.get(string)).append
			       (" with a CPU time limit of ").append
			       (Integer.toString(i)).append
			       (" seconds").toString());
	Process process = runtime.exec(strings);
	int i_8_ = process.waitFor();
	return i_8_;
    }
    
    private int plainStrategy_() throws Exception {
	return runSolver_(cManager_.getPredictedSolver(),
			  Integer.parseInt(timeLimit_));
    }
    
    private int wpeStrategy_(String string) throws Exception {
	String[] strings = solversOrder_(string);
	int i = 120;
	int i_9_ = Integer.parseInt(timeLimit_);
	boolean bool = false;
	int i_10_ = runSolver_(cManager_.getPredictedSolver(), i);
	if (i_10_ == 10 || i_10_ == 20)
	    return i_10_;
	int i_11_ = i_9_ - (strings.length * 120 - 120);
	for (int i_12_ = 0; i_12_ < strings.length - 1; i_12_++) {
	    i_10_ = runSolver_(strings[i_12_], i);
	    if (i_10_ == 10 || i_10_ == 20) {
		cManager_.retrain(strings[i_12_]);
		return i_10_;
	    }
	}
	return runSolver_(cManager_.getPredictedSolver(), i_11_);
    }
    
    private int edtStrategy_(String string) throws Exception {
	int i = Integer.parseInt(timeLimit_);
	String[] strings = solversOrder_(string);
	int i_13_ = i / strings.length;
	boolean bool = false;
	int i_14_ = runSolver_(cManager_.getPredictedSolver(), i_13_);
	if (i_14_ == 10 || i_14_ == 20)
	    return i_14_;
	for (int i_15_ = 0; i_15_ < strings.length - 1; i_15_++) {
	    i_14_ = runSolver_(strings[i_15_], i_13_);
	    if (i_14_ == 10 || i_14_ == 20) {
		cManager_.retrain(strings[i_15_]);
		return i_14_;
	    }
	}
	return i_14_;
    }
    
    private int omrrStrategy_(String string) throws Exception {
	String[] strings = solversOrder_(string);
	int i = Integer.parseInt(timeLimit_);
	int[] is = new int[4];
	is[0] = 1;
	is[1] = 10;
	is[2] = 100;
	is[3] = (i - is[0] * (strings.length + 1)
		 - is[1] * (strings.length + 1) / strings.length + 1);
	int i_16_ = 0;
	for (int i_17_ = 0; i_17_ < is.length; i_17_++) {
	    int i_18_ = is[i_17_];
	    i_16_ = runSolver_(cManager_.getPredictedSolver(), i_18_);
	    if (i_16_ == 10 || i_16_ == 20)
		return i_16_;
	    for (int i_19_ = 0; i_19_ < strings.length - 1; i_19_++) {
		i_16_ = runSolver_(strings[i_19_], i_18_);
		if (i_16_ == 10 || i_16_ == 20) {
		    cManager_.retrain(strings[i_19_]);
		    return i_16_;
		}
	    }
	}
	return i_16_;
    }
    
    private String[] solversOrder_(String string) throws Exception {
	String string_20_ = cManager_.getPredictedSolver();
	String[] strings = new String[5];
	int i = 0;
	if (string.equals("RAW")) {
	    for (int i_21_ = 0; i_21_ < rawSolverOrder_.length; i_21_++) {
		if (!rawSolverOrder_[i_21_].equals(string_20_)) {
		    strings[i] = rawSolverOrder_[i_21_];
		    i++;
		}
	    }
	} else if (string.equals("YASM")) {
	    for (int i_22_ = 0; i_22_ < yasmSolverOrder_.length; i_22_++) {
		if (!yasmSolverOrder_[i_22_].equals(string_20_)) {
		    strings[i] = yasmSolverOrder_[i_22_];
		    i++;
		}
	    }
	} else if (string.equals("RND")) {
	    for (int i_23_ = 0; i_23_ < rndSolverOrder_.length; i_23_++) {
		if (!rndSolverOrder_[i_23_].equals(string_20_)) {
		    strings[i] = rndSolverOrder_[i_23_];
		    i++;
		}
	    }
	} else if (string.equals("ALG")) {
	    if (solversMap_.get(string_20_).equals("HYBRID")) {
		for (int i_24_ = 0; i_24_ < searchSolverOrder_.length;
		     i_24_++) {
		    strings[i] = searchSolverOrder_[i_24_];
		    i++;
		}
		for (int i_25_ = 0; i_25_ < hybridSolverOrder_.length;
		     i_25_++) {
		    if (!hybridSolverOrder_[i_25_].equals(string_20_)) {
			strings[i] = hybridSolverOrder_[i_25_];
			i++;
		    }
		}
	    } else {
		for (int i_26_ = 0; i_26_ < hybridSolverOrder_.length;
		     i_26_++) {
		    strings[i] = hybridSolverOrder_[i_26_];
		    i++;
		}
		for (int i_27_ = 0; i_27_ < searchSolverOrder_.length;
		     i_27_++) {
		    if (!searchSolverOrder_[i_27_].equals(string_20_)) {
			strings[i] = searchSolverOrder_[i_27_];
			i++;
		    }
		}
	    }
	}
	return strings;
    }
    
    private int itrmStrategy_(String string) throws Exception {
	String[] strings = solversOrder_(string);
	int i = Integer.parseInt(timeLimit_);
	boolean bool = false;
	int i_28_ = 0;
	return i_28_;
    }
    
    private int trustItrStrategy_(String string) throws Exception {
	int i = 0;
	return i;
    }
}
