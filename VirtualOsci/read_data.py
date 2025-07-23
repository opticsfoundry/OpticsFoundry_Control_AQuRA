import numpy as np

def read_data(sequence_path, output_path):
    # Read the sequence list (main data)
    datain = np.loadtxt(sequence_path)
    ch = datain.T  # transpose to get columns in the right shape
    ch = ch.T      # back to original orientation (rows of data)
    
    # Read the output list (channel configuration)
    ch0_list = []
    namech = []
    with open(output_path, 'r') as f:
        while True:
            try:
                # Read 8 floats
                data_line = f.readline()
                if not data_line:
                    break
                numbers = list(map(float, data_line.strip().split()))
                if len(numbers) != 8:
                    break
                ch0_list.append(numbers)

                # Read the name (assume the name is the next line)
                name_line = f.readline().strip()
                namech.append(name_line)
            except Exception:
                break

    ch0 = np.array(ch0_list)

    # Apply the encoding logic for channel IDs
    ch0[:, 2] = ch0[:, 2] + 1_000_000 * ch0[:, 1] + (ch0[:, 2] >= 10000) * 1_000_000
    ch0[:, 2] -= (ch0[:, 2] >= 3_000_000) * 1_000_000
    ch[:, 2] = ch[:, 2] + 1_000_000 * ((ch[:, 1] != 1) + 1)  # encode analog/digital

    # Sort by channel column
    ch = ch[np.argsort(ch[:, 2])]

    return ch, ch0, namech
