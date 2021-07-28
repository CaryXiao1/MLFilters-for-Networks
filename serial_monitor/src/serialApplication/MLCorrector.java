package serialApplication;
import weka.core.DenseInstance;
import weka.core.Instance;
import weka.core.Instances;
import weka.core.converters.ArffSaver;
import weka.core.converters.ConverterUtils.DataSource;

import java.io.File;
import java.io.IOException;

import weka.classifiers.functions.SGDText;
/** Created by Cary Xiao on January 21st, 2021.
 *  Uses ML model to 
 * 
 */
public class MLCorrector {
	private SGDText classifier;
	private DataSource source; // used to log & run predictive model on error-marked text
	Instances sourceDataSet;
	
	public MLCorrector() throws Exception {
		// load variables for ML predictions
		classifier = (SGDText) weka.core.SerializationHelper.read("C:\\Users\\gameb\\OneDrive\\Desktop\\Docs\\Science Fair\\2021\\ML_Corrected_20000_word.model");
		source = new DataSource("C:\\Users\\gameb\\OneDrive\\Desktop\\Docs\\Science Fair\\2021\\ML_Word_Unsupervised.arff");
		sourceDataSet = source.getDataSet();
		System.out.println(classifier.toString());
		sourceDataSet.setClassIndex(sourceDataSet.numAttributes() - 1);
	}
	
	/* Adds & saves specified Instance to source ARFF file.
	 * @param instance - instance to be added to end of file.
	 * Returns true if file was updated, false otherwise.
	 */
	public boolean addToArff(String init, char alternate) {
		ArffSaver saver = new ArffSaver();
		
		Instance instance = new DenseInstance(sourceDataSet.get(0));
		instance.setValue(sourceDataSet.attribute(0), init);
		//instance.setValue(sourceDataSet.attribute(1), String.valueOf(alternate));
		
		sourceDataSet.add(instance);
		return true;

	}
	
	/* Uses ML model to test latest parameter in unclassified Arff file.
	 * returns the correct bit for the byte, -1 if classification failed.
	 */
	public int test() {
		try {
			double result = classifier.classifyInstance(sourceDataSet.lastInstance());
			System.out.println(result);
			return (int) result;
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return -1;
		}
	}
}
