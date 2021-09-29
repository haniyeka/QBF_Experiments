/* QbfFormulaBased - Decompiled by JODE
 * Visit http://jode.sourceforge.net/
 */

class QbfFormulaBased extends Feature
{
    public QbfFormulaBased() {
	feature_.put("EXIST_VARS", new Integer(0));
	feature_.put("FORALL_VARS", new Integer(1));
	feature_.put("TOTAL_VARS", new Integer(2));
	feature_.put("CLAUSES", new Integer(3));
	feature_.put("LITERALS", new Integer(4));
	feature_.put("EXIST_SET", new Integer(5));
	feature_.put("FORALL_SET", new Integer(6));
	feature_.put("TOTAL_SET", new Integer(7));
	feature_.put("UNARY_CLAUSES", new Integer(8));
	feature_.put("BINARY_CLAUSES", new Integer(9));
	feature_.put("TERNARY_MORE_CLAUSES", new Integer(10));
	feature_.put("POS_HORN", new Integer(11));
	feature_.put("NEG_HORN", new Integer(12));
    }
}
