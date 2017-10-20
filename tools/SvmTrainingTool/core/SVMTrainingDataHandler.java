package core;

import java.awt.image.BufferedImage;
import java.awt.image.DataBufferByte;
import java.awt.image.Raster;
import java.awt.image.WritableRaster;
import java.io.File;
import java.io.FilenameFilter;
import java.io.IOException;
import java.util.Arrays;
import java.util.HashSet;

import javax.imageio.ImageIO;

import org.opencv.core.Mat;
import org.opencv.videoio.VideoCapture;
import org.opencv.videoio.Videoio;

import util.Pair;

public class SVMTrainingDataHandler 
{
    private static final String POSITIVE_FILE_PREFIX = "SVM_TRAIN_POSITIVE_";
    private static final String NEGATIVE_FILE_PREFIX = "SVM_TRAIN_NEGATIVE_";
    private static final String TRAINING_FILE_EXTENSION = ".jpg";
    
    private int m_frameSkip;
    private int m_currentFrame;
    private int m_totalFrames;
    private int m_positiveCounter;
    private int m_negativeCounter;
    private BufferedImage m_currentFrameBuffer;
    private String m_positiveOutputPath;
    private String m_negativeOutputPath;
    private VideoCapture m_videoCapture;
    
    public SVMTrainingDataHandler()
    {
        m_frameSkip = 0;
        m_currentFrame = 0;
        m_totalFrames = 0;
        m_positiveCounter = 0;
        m_negativeCounter = 0;
        m_currentFrameBuffer = null;
        m_positiveOutputPath = null;
        m_negativeOutputPath = null;
        m_videoCapture = new VideoCapture();        
    }
    
    public boolean setSourceFile(final String filename)
    {
        boolean retVal = m_videoCapture.open(filename);
        
        if(m_videoCapture.isOpened())
        {
            m_totalFrames = (int)m_videoCapture.get(Videoio.CV_CAP_PROP_FRAME_COUNT);
            m_currentFrame = 0;
        }
        
        return retVal;
    }
    
    public void setFrameSkip(final int framesToSkip)
    {
        m_currentFrame -= m_frameSkip;
        m_currentFrame += framesToSkip;
        m_currentFrame = m_currentFrame < 0 ? 0: m_currentFrame;
        m_frameSkip = framesToSkip;
    }
    
    public boolean setPositiveOutputPath(final String path)
    {
        File directory = new File(path);
        boolean retVal = directory.isDirectory();
        
        if(retVal)
        {
            m_positiveOutputPath = path;
            m_positiveCounter = getFileCounter(m_positiveOutputPath,
                                               POSITIVE_FILE_PREFIX);
        }
        
        return retVal;
    }
    
    public boolean setNegativeOutputPath(final String path)
    {        
        File directory = new File(path);
        boolean retVal = directory.isDirectory();
    
        if(retVal)
        {
            m_negativeOutputPath = path;
            m_negativeCounter = getFileCounter(m_negativeOutputPath,
                                               NEGATIVE_FILE_PREFIX);
        }
        
        return retVal;
    }
    
    public BufferedImage getNextFrame()
    {    
        
        if(m_currentFrame < m_totalFrames)
        {
            m_videoCapture.set(Videoio.CV_CAP_PROP_POS_FRAMES, m_currentFrame);
            
            Mat nextFrame = new Mat();
            
            if(m_videoCapture.read(nextFrame))
            {
                m_currentFrameBuffer = convertMatToBufferedImage(nextFrame);
            }
            
            m_currentFrame += 1 + m_frameSkip;
        }
        else
        {
            m_currentFrameBuffer = null;
        }
        
        return m_currentFrameBuffer;
    }    
    
    public BufferedImage getCurrentFrame()
    {
        return m_currentFrameBuffer;
    }
    
    public boolean writeImagesToDisk(final HashSet< Pair<Integer, Integer> > selectedCells,
                                     final int horizontalCells,
                                     final int verticalCells,
                                     final Pair<Integer, Integer> topLeftRoI,
                                     final Pair<Integer, Integer> botRightRoI)
    {
        boolean retVal = true;
        
        if(m_positiveOutputPath != null && m_negativeOutputPath != null)
        {
            BufferedImage currentFrame = getCurrentFrame();
            Raster frameRaster = currentFrame.getRaster();
            int cellWidth = frameRaster.getWidth() /  horizontalCells;
            int cellHeight = frameRaster.getHeight() / verticalCells;
            
            for(int row = topLeftRoI.getSecond(); row <= botRightRoI.getSecond(); ++row)
            {
                if(!retVal)
                {
                    break;
                }
                
                for(int column = topLeftRoI.getFirst(); column <= botRightRoI.getFirst(); ++column)
                {
                    Pair<Integer, Integer> currentCell = new Pair<Integer, Integer>(column, row);
                    BufferedImage outImage = currentFrame.getSubimage(column * cellWidth, row * cellHeight, cellWidth, cellHeight);                    
                    
                    if(selectedCells.contains(currentCell))
                    {
                        if(!writeImageToDisk(outImage,
                                             m_positiveOutputPath,
                                             POSITIVE_FILE_PREFIX,
                                             m_positiveCounter))
                        {
                            retVal = false;
                            break;
                        }
                        else
                        {
                            ++m_positiveCounter;
                        }
                    }
                    else
                    {
                        if(!writeImageToDisk(outImage,
                                             m_negativeOutputPath,
                                             NEGATIVE_FILE_PREFIX,
                                             m_negativeCounter))
                        {
                            retVal = false;
                            break;
                        }
                        else
                        {
                            ++m_negativeCounter;
                        }
                    }
                }
            }
        }        
        else
        {
            retVal = false;
        }
        return retVal;
    }
    
    private static BufferedImage convertMatToBufferedImage(final Mat mat)
    {
        int encoding = 0;
        if (mat.channels() == 1) {
            encoding = BufferedImage.TYPE_BYTE_GRAY;
        }
        else if (mat.channels() == 3) {
            encoding = BufferedImage.TYPE_3BYTE_BGR;
        }
        
        BufferedImage image = new BufferedImage(mat.width(), mat.height(), encoding);
        WritableRaster raster = image.getRaster();
        DataBufferByte dataBuffer = (DataBufferByte) raster.getDataBuffer();
        byte[] data = dataBuffer.getData();
        mat.get(0, 0, data);

        return image;
    }
    
    private boolean writeImageToDisk(final BufferedImage image,
                                     final String path,
                                     final String filenamePrefix,
                                     final int fileCount)
    {
        boolean retVal = true;
        StringBuffer sb = new StringBuffer();
        sb.append(path);
        sb.append('/');
        sb.append(filenamePrefix);
        sb.append(String.format("%06d", fileCount));
        sb.append(TRAINING_FILE_EXTENSION);
        
        File outputfile = new File(sb.toString());
        
        try
        {
            ImageIO.write(image, "jpg", outputfile);
        } 
        catch (IOException e)
        {
            retVal = false;
        }
        
        return retVal;
    }
    
    private int getFileCounter(final String path,
                               final String filenamePrefix)
    {
        int retVal = 0;
        
        FilenameFilter fileFilter = new FilenameFilter()
                                        {
                                            public boolean accept(File dir, String name)
                                            {
                                                String lowercaseName = name.toLowerCase();
                                                if (lowercaseName.endsWith(TRAINING_FILE_EXTENSION))
                                                {
                                                    return true;
                                                } 
                                                else
                                                {
                                                    return false;
                                                }
                                            }
                                        };
                                        
        File[] fileList = new File(path).listFiles(fileFilter);
        
        if(fileList.length > 0)
        {
            Arrays.sort(fileList);
            
            String filename = fileList[fileList.length - 1].getName();
            filename = filename.substring(filenamePrefix.length(),
                                          filename.length() - TRAINING_FILE_EXTENSION.length());
            
            retVal = Integer.parseInt(filename) + 1;
        }
        
        return retVal;
    }
}