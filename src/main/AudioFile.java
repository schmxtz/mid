package main;

import java.io.File;

import javax.sound.sampled.AudioFileFormat;
import javax.sound.sampled.AudioFileFormat.Type;
import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;

public class AudioFile {
	private String fileName;
	float data[];
	
	public AudioFile() {
	}

	public String getFileName() {
		return fileName;
	}
	
	public void readFile(String fileName) {
		setFileName(fileName);
		try {
			File waveFile = new File(fileName);
			AudioFileFormat audioFileFormat = AudioSystem.getAudioFileFormat(waveFile);
			Type type = audioFileFormat.getType();
			if (type == Type.WAVE) {
				handleWaveFile(waveFile, audioFileFormat);
			} else {
				throw new Exception("Specified file type is not yet supported.");
			}
		} catch (Exception e) {
			System.out.println(e.toString());
		}
	}

	private void setFileName(String fileName) {
		this.fileName = fileName;
	}
	
	private void handleWaveFile(File audioFile, AudioFileFormat audioFileFormat) throws Exception {
		AudioFormat audioFormat = audioFileFormat.getFormat();
		AudioInputStream audioInputStream = AudioSystem.getAudioInputStream(audioFile);
		int numberOfSamples = audioFileFormat.getFrameLength();
		this.data = new float[numberOfSamples];
		byte dataBuffer[] = new byte[getBufferSize(audioFormat)];
		int sampleIndex = 0;
		while (audioInputStream.available() > 0 && sampleIndex < numberOfSamples) {
			int bytesRead = audioInputStream.read(dataBuffer);
//			for (int )
			
		}
	}
	
	private int getBufferSize(AudioFormat audioFormat) throws Exception {
		int frameSize = audioFormat.getFrameSize();
		if (frameSize == AudioSystem.NOT_SPECIFIED) {
			frameSize = calculateFrameSize(audioFormat);
		}
		int remainder = 8192 % frameSize;
		return (remainder == 0) ? 8192 : 8192 - remainder;
	}
	
	private int calculateFrameSize(AudioFormat audioFormat) throws Exception {
		int numberOfChannels = audioFormat.getChannels();
		int sampleSizeInBits = audioFormat.getSampleSizeInBits();
		if (numberOfChannels == AudioSystem.NOT_SPECIFIED || sampleSizeInBits == AudioSystem.NOT_SPECIFIED) {
			throw new Exception("FrameSize calculation fallback is missing information");
		}
		return numberOfChannels * (sampleSizeInBits / 8);
	}
}
