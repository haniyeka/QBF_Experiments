/* QbfParser - Decompiled by JODE
 * Visit http://jode.sourceforge.net/
 */
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;

class QbfParser
{
    private BufferedReader in_;
    private QbfFormula f_;
    
    public QbfParser(String string, QbfFormula qbfformula) {
	try {
	    FileReader filereader = new FileReader(string);
	    in_ = new BufferedReader(filereader);
	    f_ = qbfformula;
	} catch (FileNotFoundException filenotfoundexception) {
	    System.err.print(filenotfoundexception);
	}
    }
    
    public void parse() {
	try {
	    String string;
	    for (string = in_.readLine().trim(); string.charAt(0) == 'c';
		 string = in_.readLine().trim()) {
		/* empty */
	    }
	    if (string.charAt(0) != 'p') {
		System.out.println("ERROR: no P line");
		System.exit(0);
	    } else {
		String[] strings = string.split("\\s+");
		f_.setNvars(Integer.parseInt(strings[2].trim()));
	    }
	    string = in_.readLine().trim();
	    int i = 1;
	    Prefix prefix = new Prefix();
	    for (/**/; string.charAt(0) == 'e' || string.charAt(0) == 'a';
		 string = in_.readLine().trim()) {
		PrefixSet prefixset = new PrefixSet();
		char c;
		if (string.charAt(0) == 'e') {
		    prefixset.setExist();
		    c = 'e';
		} else {
		    prefixset.setForall();
		    c = 'a';
		}
		String[] strings = string.split("\\s+");
		for (int i_0_ = 1; i_0_ < strings.length - 1; i_0_++) {
		    int i_1_ = Integer.parseInt(strings[i_0_].trim());
		    prefixset.add(i_1_);
		    QbfVariable qbfvariable = new QbfVariable(i_1_, i, c);
		    f_.setVariable(i_1_, qbfvariable);
		}
		prefix.add(prefixset);
		i++;
	    }
	    f_.setPrefix(prefix);
	    Matrix matrix = new Matrix();
	    int i_2_ = 1;
	    for (/**/; string != null; string = string.trim()) {
		Constraint constraint = new Constraint(i_2_);
		String[] strings = string.split("\\s+");
		for (int i_3_ = 0; i_3_ < strings.length - 1; i_3_++) {
		    int i_4_ = Integer.parseInt(strings[i_3_].trim());
		    if (i_4_ > 0)
			f_.getVariable(i_4_).addPosOcc(i_2_);
		    else
			f_.getVariable(i_4_ * -1).addNegOcc(i_2_);
		    constraint.add(i_4_);
		    i_2_++;
		}
		matrix.add(constraint);
		string = in_.readLine();
		if (string == null)
		    break;
	    }
	    f_.setMatrix(matrix);
	} catch (IOException ioexception) {
	    /* empty */
	}
    }
}
