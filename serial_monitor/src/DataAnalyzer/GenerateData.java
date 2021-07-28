package DataAnalyzer;
import java.io.File;  // Import the File class
import java.io.FileNotFoundException;  // Import this class to handle errors
import java.io.FileWriter;
import java.util.ArrayList;
import java.util.Scanner; // Import the Scanner class to read text files
import java.io.IOException;

public class GenerateData {

	public static void main(String[] args) throws IOException {
		
		File file = new File("C:\\Users\\gameb\\OneDrive\\Desktop\\Docs\\Science Fair\\2021\\Testing_Sentences.txt");
		File newFile = new File("C:\\Users\\gameb\\OneDrive\\Desktop\\Docs\\Science Fair\\2021\\Testing_Sentences_Result.txt");
		Scanner scanner = new Scanner(file);
		FileWriter myWriter = new FileWriter(newFile);
		
		while(scanner.hasNextLine()) {
			ArrayList<String> words = new ArrayList<String>();
	        String sentence = scanner.nextLine();
	        int a = 0, b = 1, s = 0; // s = index of symbols
    		int[] symbolsIndex = new int[10];
    		int[] symbolsWord = new int[10];
    		
	        // scans through sentence and separates each word
	        while (b < sentence.length()) {
	        	if (sentence.charAt(b) != ' ') b++;
	        	else {
	        		
	        		String word = sentence.substring(a,b);
		        	// removes last character if not a letter or number, adds to index
		        	if (!testByteChar((byte) word.charAt(word.length() - 1))) {
		        		word = word.substring(0, word.length() - 1);
		        		symbolsIndex[s] = b - 1;
		        		symbolsWord[s] = words.size() + 1;
		        		s++;
		        	}
	        		words.add(word);
	        		a = ++b;
	        		b++;
	        	}
	        }
	        
	        // adds last word without punctuation
	        words.add(sentence.substring(a,sentence.length() - 1));
	        System.out.println(words);
    		symbolsIndex[s] = sentence.length() - 1;
    		symbolsWord[s] = words.size();
    		s++;

    		
	        // generate ML learning data for each word
	        for (int i = 0; i < words.size(); i++) {
	        	// goes through each word
	        	String word = words.get(i);
	        	System.out.println(word);
	        		
	        	for (int j = 0; j < word.length(); j++) {
	        		// goes through each character
	        		byte test = (byte) word.charAt(j);
	        		
	        		for (int k = 0; k < 8; k++) {
	        			// goes through each bit in each character
	        			byte alternate = 0;
        				// Prints out and puts data into newFile
	        			if (bitRead(test, k) == 0) {
	        				
	        				alternate = (byte) (test + Math.pow(2, 7 - k));
	        				
	        				if (testByteValid(alternate)) {
	        					System.out.println("'" + word + "','" + String.valueOf((char) alternate) + "',0");
	        					myWriter.write("'" + word + "','" + String.valueOf((char) alternate) + "',0\n");
	        				}
	        			}
	        			else if (bitRead(test, k) == 1) {
	        				System.out.println("got here");
	        				alternate = (byte) (test - Math.pow(2, 7 - k));
	        				if (testByteValid(alternate)) {
	        					String temp = word;
	        					word = word.substring(0,j) + (char) alternate + word.substring(j + 1);
	        					alternate = test;
	        					System.out.println("'" + word + "','" + String.valueOf((char) alternate) + "',1");
	        					myWriter.write("'" + word + "','" + String.valueOf((char) alternate) + "',1\n");
	        					word = temp;
	        					
	        				}
	        			}
	        		}
	        	}
	        }
	        // create cases for space errors between words
	        for (int i = 0; i < words.size() - 1; i++) {
	        	byte test = (byte) ' ';
	        	for (int j = 0; j < 8; j++) {
	        		
	        		byte alternate = 0;
	        		
	        		if (bitRead(test, j) == 0) {
        				
        				alternate = (byte) (test + Math.pow(2, 7 - j));
        				
        				if (testByteValid(alternate)) {
        					String toPrint = words.get(i) + " " + words.get(i + 1);
        					System.out.println("'" + toPrint + "','" + String.valueOf((char) alternate) + "',0");
        					myWriter.write("'" + toPrint + "','" + String.valueOf((char) alternate) + "',0\n");
        				}
        			}
        			else if (bitRead(test, j) == 1) {
        				System.out.println("got here");
        				alternate = (byte) (test - Math.pow(2, 7 - j));
        				if (testByteValid(alternate)) {
        					String toPrint = words.get(i) + String.valueOf((char) alternate) + words.get(i + 1);
        					System.out.println("'" + toPrint + "',' ',1");
        					myWriter.write("'" + toPrint + "',' ',1\n");
        				}
        			}
	        	}
	        }
	        
	        // deals with punctuation
	        for (int i = 0; i < symbolsIndex.length && symbolsIndex[i] != 0; i++) {
	        	String word = words.get(symbolsWord[i] - 1);
	        	byte test = (byte) sentence.charAt(symbolsIndex[i]);
	        	
	        	for (int j = 0; j < 8; j++) {
	        		
	        		byte alternate = 0;
	        		
	        		if (bitRead(test, j) == 0) {
        				
        				alternate = (byte) (test + Math.pow(2, 7 - j));
        				
        				if (testByteValid(alternate)) {
        					System.out.println("'" + word + String.valueOf((char) test) + "','" + String.valueOf((char) alternate) + "',0");
        					myWriter.write("'" + word + String.valueOf((char) test) + "','" + String.valueOf((char) alternate) + "',0\n");
        				}
        			}
        			else if (bitRead(test, j) == 1) {
        				System.out.println("got here");
        				alternate = (byte) (test - Math.pow(2, 7 - j));
        				if (testByteValid(alternate)) {
        					System.out.println("'" + word + String.valueOf((char) alternate) + "','" + String.valueOf((char) test) + "',1");
        					myWriter.write("'" + word + String.valueOf((char) alternate) + "','" + String.valueOf((char) test) + "',1\n");
        				}
        			}
	        	}
	        }
	        
		}
		myWriter.close();
		
	}
	private static int bitRead(byte n, int sub) {
		return (n >> sub) & 1;
	}
	// returns true if byte is valid
	private static boolean testByteValid(byte b) {
		return (b > 31 && b < 35  // space, !, "
		          || b > 43 && b < 59   // comma, -, ., 0-9, /, :
		          || b > 64 && b < 91   // A-Z
		          || b > 96 && b < 123  // a-z
		          || b == 63);          // ?
	}
	
	// returns true if byte is number or letter
	private static boolean testByteChar(byte b) {
		return (b > 47 && b < 58   // 0-9
		     || b > 64 && b < 91   // A-Z
		     || b > 96 && b < 123);// a-z
	}
	
	// returns word (w/o punctuation) of sentence at desired index
	public static String sentenceToWord(String sentence, int index) {
		
        int a = 0, b = 1, num = 0;
		
        // scans through sentence and separates each word
        while (b < sentence.length()) {
        	if (sentence.charAt(b) != ' ') b++;
        	else {
        		
        		String word = sentence.substring(a,b);
	        	// removes last character if not a letter or number, adds to index
	        	if (!testByteChar((byte) word.charAt(word.length() - 1))) {
	        		word = word.substring(0, word.length() - 1);
	        		num++; // counteracts loss of punctuation
	        	}
        		num += word.length() + 1; // counteracts loss of space
        		if (num >= index + 1) {
        			System.out.println(word);
        			return word;
        		}
        		else {
        		a = ++b;
        		b++;
        		}
        	}
        }
        // returns last word, depending on index
        if (index == sentence.length() - 1) return sentence.substring(a);
        else return sentence.substring(a, sentence.length() - 1);
	}
}
