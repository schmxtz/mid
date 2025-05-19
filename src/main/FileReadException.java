package main;

class FileReadException extends Exception {
	private static final long serialVersionUID = 4034732269319318986L;

	public FileReadException(String errorMessage) {
		super(errorMessage);
	}
}