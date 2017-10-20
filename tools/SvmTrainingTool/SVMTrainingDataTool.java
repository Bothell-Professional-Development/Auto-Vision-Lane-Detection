import org.opencv.core.Core;

import core.SVMTrainingDataToolApplication;

public class SVMTrainingDataTool {

    public static void main(String[] args) {

        System.loadLibrary( Core.NATIVE_LIBRARY_NAME);
        
        SVMTrainingDataToolApplication app = new SVMTrainingDataToolApplication();
        app.setSourceFile("C:/Users/kumader/Downloads/simpsons_movie_1080p_hddvd_trailer/2.mp4");
        app.setPositiveOutputPath("E:/SVMTraining/positive");
        app.setNegativeOutputPath("E:/SVMTraining/negative");        
        //app.setSourceFile(args[0]);
        //app.setPositiveOutputPath(args[1]);
        //app.setNegativeOutputPath(args[2]);
    }
}
