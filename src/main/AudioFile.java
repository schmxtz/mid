package main;

import java.io.File;
import java.io.IOException;
import javax.sound.sampled.AudioFileFormat;
import javax.sound.sampled.AudioFileFormat.Type;
import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.UnsupportedAudioFileException;

public class AudioFile {
	private String fileName;
	
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
	
	private void handleWaveFile(File audioFile, AudioFileFormat audioFileFormat) throws UnsupportedAudioFileException, IOException {
		System.out.println(audioFileFormat.toString());
		AudioFormat audioFormat = audioFileFormat.getFormat();
		int bufferSize = 8192 - (8192 % audioFormat.getFrameSize());
		System.out.println(audioFileFormat.getFormat().getSampleSizeInBits());
		AudioInputStream audioInputStream = AudioSystem.getAudioInputStream(audioFile);
		byte data[] = new byte[8192];
		System.out.println(audioInputStream.available());
	}
	
	private void getBufferSize(AudioFormat audioFormat) throws Exception {
		int frameSize = audioFormat.getFrameSize();
		if (frameSize == AudioSystem.NOT_SPECIFIED) {
			
		}
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
