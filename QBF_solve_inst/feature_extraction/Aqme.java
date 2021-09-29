/* Aqme - Decompiled by JODE
 * Visit http://jode.sourceforge.net/
 */

class Aqme
{
    public static void main(String[] strings) throws Exception {
        String formula = strings[0];
        QbfManager man = new QbfManager(formula);
        man.loadData();
        double features[] = man.extractFeatures();
        //for(int i = 0; i < features.length; i++) {
        //    System.out.println(features[i]);
        //}
        System.out.print(features[0] + ",");
        System.out.print(features[1] + ",");
        System.out.print(features[2] + ",");
        System.out.print(features[3] + ",");
        System.out.print(features[4] + ",");
        System.out.print(features[5] + ",");
        System.out.print(features[6] + ",");
        System.out.print(features[7] + ",");
        System.out.print(features[8] + ",");
        System.out.print(features[9] + ",");
        System.out.print(features[10] + ",");
        System.out.print(features[11] + ",");
        System.out.print(features[12] + ",");
        System.out.print(features[13] + ",");
        System.out.print(features[14] + ",");
        System.out.print(features[17] + ",");
        System.out.print(features[20] + ",");
        System.out.print(features[23] + ",");
        System.out.print(features[26] + ",");
        System.out.print(features[29] + ",");
        System.out.print(features[32] + ",");
        System.out.print(features[35] + ",");
        System.out.print(features[38] + ",");
        System.out.print(features[41] + ",");
        System.out.print(features[44] + ",");
        System.out.print(features[47] + ",");
        System.out.print(features[50] + ",");
        System.out.print(features[53] + ",");
        System.out.print(features[56] + ",");
        System.out.print(features[59] + ",");
        System.out.print(features[62] + ",");
        System.out.print(features[65] + ",");
        System.out.print(features[68] + ",");
        System.out.print(features[71] + ",");
        System.out.print(features[74] + ",");
        System.out.print(features[77] + ",");
        System.out.print(features[80] + ",");
        System.out.print(features[83] + ",");
        System.out.print(features[86] + ",");
        System.out.print(features[89] + ",");
        System.out.print(features[92] + ",");
        System.out.print(features[95] + ",");
        System.out.print(features[98] + ",");
        System.out.print(features[101] + ",");
        System.out.print(features[104] + ",");
        System.out.println(features[107]);
    }
}
