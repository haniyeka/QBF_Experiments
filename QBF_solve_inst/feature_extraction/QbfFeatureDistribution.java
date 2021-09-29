/* QbfFeatureDistribution - Decompiled by JODE
 * Visit http://jode.sourceforge.net/
 */

class QbfFeatureDistribution extends Feature
{
    public QbfFeatureDistribution() {
	feature_.put("EXIST_LIT_PER_CLAUSE", new Integer(0));
	feature_.put("FORALL_LIT_PER_CLAUSE", new Integer(1));
	feature_.put("LIT_PER_CLAUSE", new Integer(2));
	feature_.put("VARS_PER_SET", new Integer(3));
	feature_.put("EXIST_VARS_PER_SET", new Integer(4));
	feature_.put("FORALL_VARS_PER_SET", new Integer(5));
	feature_.put("POS_LITS_PER_CLAUSE", new Integer(6));
	feature_.put("FORALL_POS_LITS_PER_CLAUSE", new Integer(7));
	feature_.put("EXIST_POS_LITS_PER_CLAUSE", new Integer(8));
	feature_.put("NEG_LITS_PER_CLAUSE", new Integer(9));
	feature_.put("FORALL_NEG_LITS_PER_CLAUSE", new Integer(10));
	feature_.put("EXIST_NEG_LITS_PER_CLAUSE", new Integer(11));
	feature_.put("OCCS_NO_PER_VAR", new Integer(12));
	feature_.put("OCCS_POS_NO_PER_VAR", new Integer(13));
	feature_.put("OCCS_NEG_NO_PER_VAR", new Integer(14));
	feature_.put("OCCS_EXIST_NO_PER_VAR", new Integer(15));
	feature_.put("OCCS_FORALL_NO_PER_VAR", new Integer(16));
	feature_.put("OCCS_EXIST_POS_NO_PER_VAR", new Integer(17));
	feature_.put("OCCS_EXIST_NEG_NO_PER_VAR", new Integer(18));
	feature_.put("OCCS_FORALL_POS_NO_PER_VAR", new Integer(19));
	feature_.put("OCCS_FORALL_NEG_NO_PER_VAR", new Integer(20));
	feature_.put("W_OCCS_NO_PER_VAR", new Integer(21));
	feature_.put("W_OCCS_POS_NO_PER_VAR", new Integer(22));
	feature_.put("W_OCCS_NEG_NO_PER_VAR", new Integer(23));
	feature_.put("W_OCCS_EXIST_NO_PER_VAR", new Integer(24));
	feature_.put("W_OCCS_FORALL_NO_PER_VAR", new Integer(25));
	feature_.put("W_OCCS_EXIST_POS_NO_PER_VAR", new Integer(26));
	feature_.put("W_OCCS_EXIST_NEG_NO_PER_VAR", new Integer(27));
	feature_.put("W_OCCS_FORALL_POS_NO_PER_VAR", new Integer(28));
	feature_.put("W_OCCS_FORALL_NEG_NO_PER_VAR", new Integer(29));
	feature_.put("PRODUCTS", new Integer(30));
	feature_.put("W_PRODUCTS", new Integer(31));
    }
}
