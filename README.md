# sadsum
Simple Action Digest (SAD) - Hashing Algorithm

Internal state:

    table()
        Byte-table initialized with 65536 fractional digits of pi in base-256.

    x, y, z
        Two-byte integers representing positions within the table.
    

The algorithm:

    x = zero
    y = zero
    do
    {
        Get newByte from file
        x = x - 1
        y = y + newByte + 1
        table(x) = table(x) + table(y)

    } loop until end of file
    
    do
    {
        z = 0;
        do
        {
            y = y + (256 * table(z))
            y = y + table(x)
            table(z) = table(z) + table(y);
            z = z + 1
            
        } loop until z reaches end of table
        
        Attach copy of table(y) to end of digest
    
    } loop until digest reaches desired length



I'm converting the output to base-64, because it gives more compact
hashes than the traditional method of converting to hexadecimal.
