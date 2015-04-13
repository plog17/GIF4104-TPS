import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.input.FileSplit;

public class MyWritableComparable implements WritableComparable {
    // Some data
    private int ndf;
    private long position;

    public void write(DataOutput out) throws IOException {
        out.writeInt(ndf);
        out.writeLong(position);
    }

    public void readFields(DataInput in) throws IOException {
        ndf = in.readInt();
        position = in.readLong();
    }

    public int compareTo(MyWritableComparable o) {
        //int thisValue = this.value;
        //int thatValue = o.value;
        //return (thisValue < thatValue ? -1 : (thisValue==thatValue ? 0 : 1));
        return true;
    }

    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + ndf;
        result = prime * result + (int) (timestamp ^ (timestamp >>> 32));
        return result
    }
}