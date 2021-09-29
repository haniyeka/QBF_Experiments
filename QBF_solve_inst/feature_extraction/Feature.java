/* Feature - Decompiled by JODE
 * Visit http://jode.sourceforge.net/
 */
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

abstract class Feature
{
    protected Map feature_ = new HashMap();
    
    public Feature() {
	/* empty */
    }
    
    public int get(String string) {
	return ((Integer) feature_.get(string)).intValue();
    }
    
    public int size() {
	return feature_.size();
    }
    
    public Set labels() {
	return feature_.keySet();
    }
}
