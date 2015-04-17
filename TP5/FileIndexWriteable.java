
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.io.Writable;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.input.FileSplit;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;
import java.lang.Integer;
import java.lang.Long;
import java.lang.System;

class FileIndexWriteable implements Writable{
    public int ndf;
    public long position;

    public FileIndexWriteable(int ndf, long position){
        this.ndf = ndf;
        this.position = position;
    }

    public void write(DataOutput out) throws IOException {
        out.writeInt(ndf);
        out.writeLong(position);
    }

    public void readFields(DataInput in) throws IOException {
        ndf = in.readInt();
        position = in.readLong();
    }

    public static FileIndexWriteable read(DataInput in) throws IOException {
        FileIndexWriteable w = new FileIndexWriteable(0,0);
        w.readFields(in);
        return w;
    }

    public int compareTo(FileIndexWriteable o) {
        int fileCompare = Integer.compare(this.ndf, o.ndf);
        int positionCompare = Long.compare(this.position, o.position);
        //return (thisValue < thatValue ? -1 : (thisValue==thatValue ? 0 : 1));
        return positionCompare;
    }

    public String toString(){
        return this.ndf + " " + this.position;
    }

    public int hashCode() {
        final int prime = 31;
        int result = 1;
        long timestamp = System.currentTimeMillis();
        result = prime * result + ndf;
        result = prime * result + (int) (timestamp ^ (timestamp >>> 32));
        return result;
    }
}