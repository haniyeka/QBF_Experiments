/* QbfRatio - Decompiled by JODE
 * Visit http://jode.sourceforge.net/
 */

class QbfRatio extends Feature
{
    public QbfRatio() {
	feature_.put("CLAUSE_VAR", new Integer(0));
	feature_.put("LITP_LIT", new Integer(1));
	feature_.put("LITN_LIT", new Integer(2));
	feature_.put("LITP_LITN", new Integer(3));
	feature_.put("LITE_LIT", new Integer(4));
	feature_.put("LITEP_LIT", new Integer(5));
	feature_.put("LITEN_LIT", new Integer(6));
	feature_.put("LITEP_LITE", new Integer(7));
	feature_.put("LITEN_LITE", new Integer(8));
	feature_.put("LITEP_LITEN", new Integer(9));
	feature_.put("LITEP_LITP", new Integer(10));
	feature_.put("LITEN_LITN", new Integer(11));
	feature_.put("LITF_LIT", new Integer(12));
	feature_.put("LITFP_LIT", new Integer(13));
	feature_.put("LITFN_LIT", new Integer(14));
	feature_.put("LITFP_LITF", new Integer(15));
	feature_.put("LITFN_LITF", new Integer(16));
	feature_.put("LITFP_LITFN", new Integer(17));
	feature_.put("LITFP_LITP", new Integer(18));
	feature_.put("LITFN_LITN", new Integer(19));
	feature_.put("OCCP_OCCN", new Integer(20));
	feature_.put("OCCP_OCC", new Integer(21));
	feature_.put("OCCN_OCC", new Integer(22));
	feature_.put("OCCE_OCC", new Integer(23));
	feature_.put("OCCF_OCC", new Integer(24));
	feature_.put("OCCEP_OCC", new Integer(25));
	feature_.put("OCCEN_OCC", new Integer(26));
	feature_.put("OCCFP_OCC", new Integer(27));
	feature_.put("OCCFN_OCC", new Integer(28));
	feature_.put("OCCFP_OCCF", new Integer(29));
	feature_.put("OCCFN_OCCF", new Integer(30));
	feature_.put("OCCEP_OCCE", new Integer(31));
	feature_.put("OCCEN_OCCE", new Integer(32));
	feature_.put("OCCEP_OCCEN", new Integer(33));
	feature_.put("OCCEP_OCCP", new Integer(34));
	feature_.put("OCCEN_OCCN", new Integer(35));
	feature_.put("OCCFP_OCCP", new Integer(36));
	feature_.put("OCCFN_OCCN", new Integer(37));
	feature_.put("OCCFP_OCCFN", new Integer(38));
	feature_.put("UNARY_CLAUSE", new Integer(39));
	feature_.put("BINARY_CLAUSE", new Integer(40));
	feature_.put("TERMORE_CLAUSE", new Integer(41));
	feature_.put("POS_NEG_HORN", new Integer(42));
	feature_.put("POS_HORN_CLAUSE", new Integer(43));
	feature_.put("NEG_HORN_CLAUSE", new Integer(44));
	feature_.put("WOCCP_WOCCN", new Integer(45));
	feature_.put("WOCCP_WOCC", new Integer(46));
	feature_.put("WOCCN_WOCC", new Integer(47));
	feature_.put("WOCCE_WOCC", new Integer(48));
	feature_.put("WOCCF_WOCC", new Integer(49));
	feature_.put("WOCCEP_WOCC", new Integer(50));
	feature_.put("WOCCEN_WOCC", new Integer(51));
	feature_.put("WOCCFP_WOCC", new Integer(52));
	feature_.put("WOCCFN_WOCC", new Integer(53));
	feature_.put("WOCCFP_WOCCF", new Integer(54));
	feature_.put("WOCCFN_WOCCF", new Integer(55));
	feature_.put("WOCCEP_WOCCE", new Integer(56));
	feature_.put("WOCCEN_WOCCE", new Integer(57));
	feature_.put("WOCCEP_WOCCEN", new Integer(58));
	feature_.put("WOCCEP_WOCCP", new Integer(59));
	feature_.put("WOCCEN_WOCCN", new Integer(60));
	feature_.put("WOCCFP_WOCCP", new Integer(61));
	feature_.put("WOCCFN_WOCCN", new Integer(62));
	feature_.put("WOCCFP_WOCCFN", new Integer(63));
    }
}
