/* FeatureManager - Decompiled by JODE
 * Visit http://jode.sourceforge.net/
 */
import java.util.Iterator;
import java.util.Vector;

abstract class FeatureManager
{
    protected Vector featureValues_ = new Vector();
    protected Vector featureDistributions_ = new Vector();
    protected Vector featureStatistics_ = new Vector();
    protected Vector featureRatio_ = new Vector();
    
    public FeatureManager() {
	/* empty */
    }
    
    protected double division_(double d, double d_0_) {
	if (d == 0.0 || d_0_ == 0.0 || d / d_0_ < 0.0)
	    return 0.0;
	return d / d_0_;
    }
    
    protected double division_(int i, int i_1_) {
	if (i == 0 || i_1_ == 0 || i / i_1_ < 0)
	    return 0.0;
	return (double) (i / i_1_);
    }
    
    protected double division_(int i, double d) {
	if (i == 0 || d == 0.0 || (double) i / d < 0.0)
	    return 0.0;
	return (double) i / d;
    }
    
    protected double mean_(Vector vector) {
	double d = (double) vector.size();
	if (d == 0.0)
	    return 0.0;
	double d_2_ = 0.0;
	Iterator iterator = vector.iterator();
	while (iterator.hasNext())
	    d_2_ += ((Double) iterator.next()).doubleValue();
	return d_2_ / d;
    }
    
    protected double sdev_(Vector vector) {
	double d = (double) vector.size();
	if (d == 0.0)
	    return 0.0;
	double d_3_ = mean_(vector);
	double d_4_ = 0.0;
	for (int i = 0; i < (int) d; i++)
	    d_4_ += (((Double) vector.get(i)).doubleValue()
		     * ((Double) vector.get(i)).doubleValue()) - d_3_ * d_3_;
	double d_5_ = Math.sqrt(d_4_ / d);
	return d_5_;
    }
}
