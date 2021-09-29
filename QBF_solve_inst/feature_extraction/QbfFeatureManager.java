/* QbfFeatureManager - Decompiled by JODE
 * Visit http://jode.sourceforge.net/
 */
import java.util.Iterator;
import java.util.Vector;

class QbfFeatureManager extends FeatureManager
{
    private QbfFormulaBased fb_ = new QbfFormulaBased();
    private QbfFeatureDistribution fd_ = new QbfFeatureDistribution();
    private QbfRatio r_ = new QbfRatio();
    private Statistics s_ = new Statistics();
    private QbfFormula f_;
    
    public QbfFeatureManager(QbfFormula qbfformula) {
	f_ = qbfformula;
	featureValues_.setSize(fb_.size());
	for (int i = 0; i < featureValues_.size(); i++)
	    featureValues_.set(i, Double.valueOf(0.0));
	for (int i = 0; i < fd_.size(); i++) {
	    Vector vector = new Vector(0);
	    featureDistributions_.add(vector);
	    Vector vector_0_ = new Vector();
	    vector_0_.setSize(s_.size());
	    featureStatistics_.add(vector_0_);
	}
	featureRatio_.setSize(r_.size());
    }
    
    public void computeFeatures() {
	computePrefixBaseFeatures_();
	computeMatrixBaseFeatures_();
	computeStatisticsFeatures_();
	computeRatioFeatures_();
    }
    
    public void print() {
	for (int i = 0; i < featureValues_.size(); i++)
	    System.out.print(new StringBuilder().append(",").append
				 (featureValues_.get(i)).toString());
	for (int i = 0; i < featureDistributions_.size(); i++)
	    System.out.print(new StringBuilder().append(",").append
				 (((Vector) featureStatistics_.get(i))
				      .get(s_.get("MEAN")))
				 .toString());
	for (int i = 0; i < featureRatio_.size(); i++)
	    System.out.print(new StringBuilder().append(",").append
				 (featureRatio_.get(i)).toString());
	System.out.println("");
    }
    
    public double[] getFeatures() {
	int i = (featureValues_.size() + featureDistributions_.size()
		 + featureRatio_.size());
	double[] ds = new double[i];
	int i_1_ = 0;
	for (int i_2_ = 0; i_2_ < featureValues_.size(); i_2_++) {
	    ds[i_1_] = ((Double) featureValues_.get(i_2_)).doubleValue();
	    i_1_++;
	}
	for (int i_3_ = 0; i_3_ < featureDistributions_.size(); i_3_++) {
	    ds[i_1_]
		= ((Double)
		   ((Vector) featureStatistics_.get(i_3_)).get(s_.get("MEAN")))
		      .doubleValue();
	    i_1_++;
	}
	for (int i_4_ = 0; i_4_ < featureRatio_.size(); i_4_++) {
	    ds[i_1_] = ((Double) featureRatio_.get(i_4_)).doubleValue();
	    i_1_++;
	}
	return ds;
    }
    
    private void computePrefixBaseFeatures_() {
	Vector vector = new Vector();
	Vector vector_5_ = new Vector();
	Vector vector_6_ = new Vector();
	Prefix prefix = f_.getPrefix();
	Iterator iterator = prefix.itr();
	while (iterator.hasNext()) {
	    PrefixSet prefixset = (PrefixSet) iterator.next();
	    vector.add(Double.valueOf((double) prefixset.size()));
	    if (prefixset.isExist()) {
		featureValues_.set
		    (fb_.get("EXIST_SET"),
		     Double.valueOf(((Double)
				     featureValues_.get(fb_.get("EXIST_SET")))
					.doubleValue() + 1.0));
		featureValues_.set
		    (fb_.get("EXIST_VARS"),
		     Double.valueOf(((Double)
				     featureValues_.get(fb_.get("EXIST_VARS")))
					.doubleValue()
				    + (double) prefixset.size()));
		vector_5_.add(Double.valueOf((double) prefixset.size()));
	    } else if (prefixset.isForall()) {
		featureValues_.set
		    (fb_.get("FORALL_SET"),
		     Double.valueOf(((Double)
				     featureValues_.get(fb_.get("FORALL_SET")))
					.doubleValue() + 1.0));
		featureValues_.set
		    (fb_.get("FORALL_VARS"),
		     Double.valueOf(((Double) featureValues_
						  .get(fb_.get("FORALL_VARS")))
					.doubleValue()
				    + (double) prefixset.size()));
		vector_6_.add(Double.valueOf((double) prefixset.size()));
	    }
	}
	featureValues_.set(fb_.get("TOTAL_VARS"),
			   Double.valueOf(((Double)
					   featureValues_
					       .get(fb_.get("FORALL_VARS")))
					      .doubleValue()
					  + ((Double)
					     featureValues_
						 .get(fb_.get("EXIST_VARS")))
						.doubleValue()));
	featureValues_.set(fb_.get("TOTAL_SET"),
			   Double.valueOf(((Double)
					   featureValues_
					       .get(fb_.get("FORALL_SET")))
					      .doubleValue()
					  + ((Double)
					     featureValues_
						 .get(fb_.get("EXIST_SET")))
						.doubleValue()));
	featureDistributions_.set(fd_.get("VARS_PER_SET"), vector);
	featureDistributions_.set(fd_.get("EXIST_VARS_PER_SET"), vector_5_);
	featureDistributions_.set(fd_.get("FORALL_VARS_PER_SET"), vector_6_);
    }
    
    private void computeMatrixBaseFeatures_() {
	Matrix matrix = f_.getMatrix();
	Vector vector = f_.getVariables();
	Vector vector_7_ = new Vector();
	Vector vector_8_ = new Vector();
	Vector vector_9_ = new Vector();
	Vector vector_10_ = new Vector();
	Vector vector_11_ = new Vector();
	Vector vector_12_ = new Vector();
	Vector vector_13_ = new Vector();
	Vector vector_14_ = new Vector();
	Vector vector_15_ = new Vector();
	Vector vector_16_ = new Vector();
	Vector vector_17_ = new Vector();
	Vector vector_18_ = new Vector();
	Vector vector_19_ = new Vector();
	Vector vector_20_ = new Vector();
	Vector vector_21_ = new Vector();
	Vector vector_22_ = new Vector();
	Vector vector_23_ = new Vector();
	Vector vector_24_ = new Vector();
	Vector vector_25_ = new Vector();
	Vector vector_26_ = new Vector();
	Vector vector_27_ = new Vector();
	Vector vector_28_ = new Vector();
	Vector vector_29_ = new Vector();
	Vector vector_30_ = new Vector();
	Vector vector_31_ = new Vector();
	Vector vector_32_ = new Vector();
	Vector vector_33_ = new Vector();
	Vector vector_34_ = new Vector();
	Vector vector_35_ = new Vector();
	Iterator iterator = matrix.itr();
	while (iterator.hasNext()) {
	    featureValues_.set(fb_.get("CLAUSES"),
			       Double.valueOf(((Double)
					       featureValues_
						   .get(fb_.get("CLAUSES")))
						  .doubleValue() + 1.0));
	    Constraint constraint = (Constraint) iterator.next();
	    if (constraint.size() == 1)
		featureValues_.set
		    (fb_.get("UNARY_CLAUSES"),
		     Double.valueOf(((Double)
				     featureValues_
					 .get(fb_.get("UNARY_CLAUSES")))
					.doubleValue() + 1.0));
	    else if (constraint.size() == 2)
		featureValues_.set
		    (fb_.get("BINARY_CLAUSES"),
		     Double.valueOf(((Double)
				     featureValues_
					 .get(fb_.get("BINARY_CLAUSES")))
					.doubleValue() + 1.0));
	    else
		featureValues_.set
		    (fb_.get("TERNARY_MORE_CLAUSES"),
		     Double.valueOf(((Double)
				     featureValues_
					 .get(fb_.get("TERNARY_MORE_CLAUSES")))
					.doubleValue() + 1.0));
	    int i = 0;
	    int i_36_ = 0;
	    int i_37_ = 0;
	    int i_38_ = 0;
	    int i_39_ = 0;
	    int i_40_ = 0;
	    int i_41_ = 0;
	    int i_42_ = 0;
	    int i_43_ = 0;
	    Iterator iterator_44_ = constraint.itr();
	    while (iterator_44_.hasNext()) {
		int i_45_ = ((Integer) iterator_44_.next()).intValue();
		featureValues_.set
		    (fb_.get("LITERALS"),
		     Double.valueOf(((Double)
				     featureValues_.get(fb_.get("LITERALS")))
					.doubleValue() + 1.0));
		i++;
		if (i_45_ > 0)
		    i_36_++;
		else
		    i_37_++;
		if (((QbfVariable) vector.get(Math.abs(i_45_))).isExist())
		    i_38_++;
		else
		    i_39_++;
		if (i_45_ > 0
		    && ((QbfVariable) vector.get(Math.abs(i_45_))).isExist())
		    i_40_++;
		else if (i_45_ < 0
			 && ((QbfVariable) vector.get(Math.abs(i_45_)))
				.isExist())
		    i_41_++;
		else if (i_45_ > 0
			 && ((QbfVariable) vector.get(Math.abs(i_45_)))
				.isForall())
		    i_42_++;
		else if (i_45_ < 0
			 && ((QbfVariable) vector.get(Math.abs(i_45_)))
				.isForall())
		    i_43_++;
	    }
	    if (i_36_ < 2)
		featureValues_.set
		    (fb_.get("POS_HORN"),
		     Double.valueOf(((Double)
				     featureValues_.get(fb_.get("POS_HORN")))
					.doubleValue() + 1.0));
	    if (i_37_ < 2)
		featureValues_.set
		    (fb_.get("NEG_HORN"),
		     Double.valueOf(((Double)
				     featureValues_.get(fb_.get("NEG_HORN")))
					.doubleValue() + 1.0));
	    vector_7_.add(Double.valueOf((double) i));
	    vector_8_.add(Double.valueOf((double) i_36_));
	    vector_9_.add(Double.valueOf((double) i_37_));
	    vector_10_.add(Double.valueOf((double) i_38_));
	    vector_11_.add(Double.valueOf((double) i_39_));
	    vector_12_.add(Double.valueOf((double) i_40_));
	    vector_13_.add(Double.valueOf((double) i_41_));
	    vector_14_.add(Double.valueOf((double) i_42_));
	    vector_15_.add(Double.valueOf((double) i_43_));
	}
	iterator = vector.iterator();
	while (iterator.hasNext()) {
	    QbfVariable qbfvariable = (QbfVariable) iterator.next();
	    if (qbfvariable != null) {
		int i = qbfvariable.posOccs() + qbfvariable.negOccs();
		vector_16_.add(Double.valueOf((double) i));
		vector_25_.add(Double.valueOf((double) (qbfvariable.getLevel()
							* i)));
		vector_17_.add(Double.valueOf((double) qbfvariable.posOccs()));
		vector_26_.add(Double.valueOf((double) (qbfvariable.getLevel()
							* qbfvariable
							      .posOccs())));
		vector_18_.add(Double.valueOf((double) qbfvariable.negOccs()));
		vector_27_.add(Double.valueOf((double) (qbfvariable.getLevel()
							* qbfvariable
							      .negOccs())));
		if (qbfvariable.isExist()) {
		    vector_19_.add(Double.valueOf((double) i));
		    vector_28_.add(Double.valueOf((double) (qbfvariable
								.getLevel()
							    * i)));
		    vector_21_
			.add(Double.valueOf((double) qbfvariable.posOccs()));
		    vector_30_.add
			(Double.valueOf((double) (qbfvariable.getLevel()
						  * qbfvariable.posOccs())));
		    vector_22_
			.add(Double.valueOf((double) qbfvariable.negOccs()));
		    vector_31_.add
			(Double.valueOf((double) (qbfvariable.getLevel()
						  * qbfvariable.negOccs())));
		    vector_34_.add
			(Double.valueOf((double) (qbfvariable.posOccs()
						  * qbfvariable.negOccs())));
		    vector_35_.add
			(Double.valueOf((double) (qbfvariable.getLevel()
						  * qbfvariable.posOccs()
						  * qbfvariable.negOccs())));
		} else {
		    vector_20_.add(Double.valueOf((double) i));
		    vector_29_.add(Double.valueOf((double) (qbfvariable
								.getLevel()
							    * i)));
		    vector_23_
			.add(Double.valueOf((double) qbfvariable.posOccs()));
		    vector_32_.add
			(Double.valueOf((double) (qbfvariable.getLevel()
						  * qbfvariable.posOccs())));
		    vector_24_
			.add(Double.valueOf((double) qbfvariable.negOccs()));
		    vector_33_.add
			(Double.valueOf((double) (qbfvariable.getLevel()
						  * qbfvariable.negOccs())));
		}
	    }
	}
	featureDistributions_.set(fd_.get("EXIST_LIT_PER_CLAUSE"), vector_10_);
	featureDistributions_.set(fd_.get("FORALL_LIT_PER_CLAUSE"),
				  vector_11_);
	featureDistributions_.set(fd_.get("LIT_PER_CLAUSE"), vector_7_);
	featureDistributions_.set(fd_.get("POS_LITS_PER_CLAUSE"), vector_8_);
	featureDistributions_.set(fd_.get("FORALL_POS_LITS_PER_CLAUSE"),
				  vector_14_);
	featureDistributions_.set(fd_.get("EXIST_POS_LITS_PER_CLAUSE"),
				  vector_12_);
	featureDistributions_.set(fd_.get("NEG_LITS_PER_CLAUSE"), vector_9_);
	featureDistributions_.set(fd_.get("FORALL_NEG_LITS_PER_CLAUSE"),
				  vector_15_);
	featureDistributions_.set(fd_.get("EXIST_NEG_LITS_PER_CLAUSE"),
				  vector_13_);
	featureDistributions_.set(fd_.get("OCCS_NO_PER_VAR"), vector_16_);
	featureDistributions_.set(fd_.get("OCCS_POS_NO_PER_VAR"), vector_17_);
	featureDistributions_.set(fd_.get("OCCS_NEG_NO_PER_VAR"), vector_18_);
	featureDistributions_.set(fd_.get("OCCS_EXIST_NO_PER_VAR"),
				  vector_19_);
	featureDistributions_.set(fd_.get("OCCS_FORALL_NO_PER_VAR"),
				  vector_20_);
	featureDistributions_.set(fd_.get("OCCS_EXIST_POS_NO_PER_VAR"),
				  vector_21_);
	featureDistributions_.set(fd_.get("OCCS_EXIST_NEG_NO_PER_VAR"),
				  vector_22_);
	featureDistributions_.set(fd_.get("OCCS_FORALL_POS_NO_PER_VAR"),
				  vector_23_);
	featureDistributions_.set(fd_.get("OCCS_FORALL_NEG_NO_PER_VAR"),
				  vector_24_);
	featureDistributions_.set(fd_.get("W_OCCS_NO_PER_VAR"), vector_25_);
	featureDistributions_.set(fd_.get("W_OCCS_POS_NO_PER_VAR"),
				  vector_26_);
	featureDistributions_.set(fd_.get("W_OCCS_NEG_NO_PER_VAR"),
				  vector_27_);
	featureDistributions_.set(fd_.get("W_OCCS_EXIST_NO_PER_VAR"),
				  vector_28_);
	featureDistributions_.set(fd_.get("W_OCCS_FORALL_NO_PER_VAR"),
				  vector_29_);
	featureDistributions_.set(fd_.get("W_OCCS_EXIST_POS_NO_PER_VAR"),
				  vector_30_);
	featureDistributions_.set(fd_.get("W_OCCS_EXIST_NEG_NO_PER_VAR"),
				  vector_31_);
	featureDistributions_.set(fd_.get("W_OCCS_FORALL_POS_NO_PER_VAR"),
				  vector_32_);
	featureDistributions_.set(fd_.get("W_OCCS_FORALL_NEG_NO_PER_VAR"),
				  vector_33_);
	featureDistributions_.set(fd_.get("PRODUCTS"), vector_34_);
	featureDistributions_.set(fd_.get("W_PRODUCTS"), vector_35_);
    }
    
    private void computeStatisticsFeatures_() {
	for (int i = 0; i < fd_.size(); i++) {
	    double d = mean_((Vector) featureDistributions_.get(i));
	    Vector vector = new Vector();
	    vector.add(Double.valueOf(d));
	    featureStatistics_.set(i, vector);
	}
    }
    
    private void computeRatioFeatures_() {
	featureRatio_.set
	    (r_.get("CLAUSE_VAR"),
	     Double.valueOf(division_(((Double)
				       featureValues_.get(fb_.get("CLAUSES")))
					  .doubleValue(),
				      ((Double)
				       featureValues_
					   .get(fb_.get("TOTAL_VARS")))
					  .doubleValue())));
	featureRatio_.set
	    (r_.get("LITP_LIT"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("POS_LITS_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector)
			   featureStatistics_.get(fd_.get("LIT_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("LITN_LIT"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("NEG_LITS_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector)
			   featureStatistics_.get(fd_.get("LIT_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("LITP_LITN"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("POS_LITS_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("NEG_LITS_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("LITE_LIT"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("EXIST_LIT_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector)
			   featureStatistics_.get(fd_.get("LIT_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("LITEP_LIT"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("EXIST_POS_LITS_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector)
			   featureStatistics_.get(fd_.get("LIT_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("LITEN_LIT"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("EXIST_NEG_LITS_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector)
			   featureStatistics_.get(fd_.get("LIT_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("LITEP_LITE"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("EXIST_POS_LITS_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("EXIST_LIT_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("LITEN_LITE"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("EXIST_NEG_LITS_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("EXIST_LIT_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("LITEP_LITEN"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("EXIST_POS_LITS_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("EXIST_NEG_LITS_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("LITEP_LITP"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("EXIST_POS_LITS_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("POS_LITS_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("LITEN_LITN"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("EXIST_NEG_LITS_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("NEG_LITS_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("LITF_LIT"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("FORALL_LIT_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector)
			   featureStatistics_.get(fd_.get("LIT_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("LITFP_LIT"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("FORALL_POS_LITS_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector)
			   featureStatistics_.get(fd_.get("LIT_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("LITFN_LIT"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("FORALL_NEG_LITS_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector)
			   featureStatistics_.get(fd_.get("LIT_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("LITFP_LITF"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("FORALL_POS_LITS_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("FORALL_LIT_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("LITFN_LITF"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("FORALL_NEG_LITS_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("FORALL_LIT_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("LITFP_LITFN"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("FORALL_POS_LITS_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("FORALL_NEG_LITS_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("LITFP_LITP"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("FORALL_POS_LITS_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("POS_LITS_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("LITFN_LITN"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("FORALL_NEG_LITS_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("NEG_LITS_PER_CLAUSE")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("OCCP_OCCN"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("OCCS_POS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("OCCS_NEG_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("OCCP_OCC"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("OCCS_POS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector)
			   featureStatistics_.get(fd_.get("OCCS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("OCCN_OCC"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("OCCS_NEG_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector)
			   featureStatistics_.get(fd_.get("OCCS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("OCCE_OCC"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("OCCS_EXIST_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector)
			   featureStatistics_.get(fd_.get("OCCS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("OCCF_OCC"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("OCCS_FORALL_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector)
			   featureStatistics_.get(fd_.get("OCCS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("OCCEP_OCC"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("OCCS_EXIST_POS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector)
			   featureStatistics_.get(fd_.get("OCCS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("OCCEN_OCC"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("OCCS_EXIST_NEG_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector)
			   featureStatistics_.get(fd_.get("OCCS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("OCCFP_OCC"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("OCCS_FORALL_POS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector)
			   featureStatistics_.get(fd_.get("OCCS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("OCCFN_OCC"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("OCCS_FORALL_NEG_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector)
			   featureStatistics_.get(fd_.get("OCCS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("OCCFP_OCCF"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("OCCS_FORALL_POS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("OCCS_FORALL_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("OCCFN_OCCF"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("OCCS_FORALL_NEG_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("OCCS_FORALL_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("OCCEP_OCCE"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("OCCS_EXIST_POS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("OCCS_EXIST_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("OCCEN_OCCE"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("OCCS_EXIST_NEG_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("OCCS_EXIST_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("OCCEP_OCCEN"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("OCCS_EXIST_POS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("OCCS_EXIST_NEG_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("OCCEP_OCCP"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("OCCS_EXIST_POS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("OCCS_POS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("OCCEN_OCCN"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("OCCS_EXIST_NEG_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("OCCS_NEG_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("OCCFP_OCCP"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("OCCS_FORALL_POS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("OCCS_POS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("OCCFN_OCCN"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("OCCS_FORALL_NEG_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("OCCS_NEG_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("OCCFP_OCCFN"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("OCCS_FORALL_POS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("OCCS_FORALL_NEG_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("UNARY_CLAUSE"),
	     Double.valueOf(division_(((Double)
				       featureValues_
					   .get(fb_.get("UNARY_CLAUSES")))
					  .doubleValue(),
				      ((Double)
				       featureValues_.get(fb_.get("CLAUSES")))
					  .doubleValue())));
	featureRatio_.set
	    (r_.get("BINARY_CLAUSE"),
	     Double.valueOf(division_(((Double)
				       featureValues_
					   .get(fb_.get("BINARY_CLAUSES")))
					  .doubleValue(),
				      ((Double)
				       featureValues_.get(fb_.get("CLAUSES")))
					  .doubleValue())));
	featureRatio_.set
	    (r_.get("TERMORE_CLAUSE"),
	     (Double.valueOf
	      (division_(((Double)
			  featureValues_.get(fb_.get("TERNARY_MORE_CLAUSES")))
			     .doubleValue(),
			 ((Double) featureValues_.get(fb_.get("CLAUSES")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("POS_NEG_HORN"),
	     Double.valueOf(division_(((Double)
				       featureValues_.get(fb_.get("POS_HORN")))
					  .doubleValue(),
				      ((Double)
				       featureValues_.get(fb_.get("NEG_HORN")))
					  .doubleValue())));
	featureRatio_.set
	    (r_.get("POS_HORN_CLAUSE"),
	     Double.valueOf(division_(((Double)
				       featureValues_.get(fb_.get("POS_HORN")))
					  .doubleValue(),
				      ((Double)
				       featureValues_.get(fb_.get("CLAUSES")))
					  .doubleValue())));
	featureRatio_.set
	    (r_.get("NEG_HORN_CLAUSE"),
	     Double.valueOf(division_(((Double)
				       featureValues_.get(fb_.get("NEG_HORN")))
					  .doubleValue(),
				      ((Double)
				       featureValues_.get(fb_.get("CLAUSES")))
					  .doubleValue())));
	featureRatio_.set
	    (r_.get("WOCCP_WOCCN"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("W_OCCS_POS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("W_OCCS_NEG_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("WOCCP_WOCC"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("W_OCCS_POS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("W_OCCS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("WOCCN_WOCC"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("W_OCCS_NEG_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("W_OCCS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("WOCCE_WOCC"),
	     Double.valueOf(division_(((Double)
				       ((Vector)
					(featureStatistics_.get
					 (fd_.get("W_OCCS_EXIST_NO_PER_VAR"))))
					   .get(s_.get("MEAN")))
					  .doubleValue(),
				      ((Double)
				       ((Vector)
					featureStatistics_
					    .get(fd_.get("W_OCCS_NO_PER_VAR")))
					   .get(s_.get("MEAN")))
					  .doubleValue())));
	featureRatio_.set
	    (r_.get("WOCCF_WOCC"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("W_OCCS_FORALL_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("W_OCCS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("WOCCEP_WOCC"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("W_OCCS_EXIST_POS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("W_OCCS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("WOCCEN_WOCC"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("W_OCCS_EXIST_NEG_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("W_OCCS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("WOCCFP_WOCC"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("W_OCCS_FORALL_POS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("W_OCCS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("WOCCFN_WOCC"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("W_OCCS_FORALL_NEG_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("W_OCCS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("WOCCFP_WOCCF"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("W_OCCS_FORALL_POS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("W_OCCS_FORALL_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("WOCCFN_WOCCF"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("W_OCCS_FORALL_NEG_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("W_OCCS_FORALL_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("WOCCEP_WOCCE"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("W_OCCS_EXIST_POS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("W_OCCS_EXIST_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("WOCCEN_WOCCE"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("W_OCCS_EXIST_NEG_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("W_OCCS_EXIST_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("WOCCEP_WOCCEN"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("W_OCCS_EXIST_POS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("W_OCCS_EXIST_NEG_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("WOCCEP_WOCCP"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("W_OCCS_EXIST_POS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("W_OCCS_POS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("WOCCEN_WOCCN"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("W_OCCS_EXIST_NEG_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("W_OCCS_NEG_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("WOCCFP_WOCCP"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("W_OCCS_FORALL_POS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("W_OCCS_POS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("WOCCFN_WOCCN"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("W_OCCS_FORALL_NEG_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector) featureStatistics_
					.get(fd_.get("W_OCCS_NEG_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
	featureRatio_.set
	    (r_.get("WOCCFP_WOCCFN"),
	     (Double.valueOf
	      (division_(((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("W_OCCS_FORALL_POS_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue(),
			 ((Double)
			  ((Vector)
			   featureStatistics_
			       .get(fd_.get("W_OCCS_FORALL_NEG_NO_PER_VAR")))
			      .get(s_.get("MEAN")))
			     .doubleValue()))));
    }
}
