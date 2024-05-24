### pip install pydub numpy

from pydub import AudioSegment
from pydub.generators import Sine

# Generate a sine wave beep sound
frequency = 840  # Frequency in Hz 
duration = 1000/60  # Duration in milliseconds (1/60 second)

sine_wave = Sine(frequency).to_audio_segment(duration=duration)

# Export the sine wave as a WAV file
sine_wave.export("beep.wav", format="wav")


def generate_cpp_byte_array(file_path, variable_name):
    with open(file_path, 'rb') as file:
        byte_data = file.read()

    print(f"std::vector<uint8_t> {variable_name} = {{")
    for i, byte in enumerate(byte_data):
        if i % 12 == 0:
            print()
        print(f"0x{byte:02X}, ", end='')
    print("\n};")

generate_cpp_byte_array('beep.wav', 'beepSoundData')


