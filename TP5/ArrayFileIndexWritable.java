import org.apache.hadoop.io.ArrayWritable;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.input.FileSplit;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.io.Writable;

import java.io.DataOutput;
import java.io.IOException;
import java.util.Arrays;

class ArrayFileIndexWritable extends ArrayWritable{

    public ArrayFileIndexWritable(Class<? extends Writable> valueClass, Writable[] values) {
        super(valueClass, values);
    }
    public ArrayFileIndexWritable(Class<? extends Writable> valueClass) {
        super(valueClass);
    }

    @Override
    public FileIndexWritable[] get() {
        return (FileIndexWritable[]) super.get();
    }

    @Override
    public void write(DataOutput arg0) throws IOException {
        super.write(arg0);
    }

    @Override
    public String toString() {
        return Arrays.toString(get());
    }
}