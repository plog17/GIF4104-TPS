
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

import java.io.IOException;
import java.lang.String;
import java.util.ArrayList;
import java.util.StringTokenizer;

public class WordCount {

  public static class TokenizerMapper extends Mapper<LongWritable, Text, Text, FileIndexWritable>{

    private Text word = new Text();
    String fileName = new String();

    public void map(LongWritable key, Text value, Context context) throws IOException, InterruptedException {
      StringTokenizer itr = new StringTokenizer(value.toString());
      
      fileName = ((FileSplit) context.getInputSplit()).getPath().toString();
      int index = Character.getNumericValue(fileName.charAt(fileName.length() - 5)); 

      while (itr.hasMoreTokens()) {
        word.set(itr.nextToken());
        FileIndexWritable fileValue = new FileIndexWritable(index, key.get());
        context.write(word, fileValue);
      }
    }
  }

  public static class IntSumReducer
       extends Reducer<Text,FileIndexWritable,Text,ArrayFileIndexWritable> {
    public void reduce(Text key, Iterable<FileIndexWritable> values, Context context
                       ) throws IOException, InterruptedException {
      ArrayList<FileIndexWritable> fileList = new ArrayList<>();
      for (FileIndexWritable val : values) {
        fileList.add(new FileIndexWritable(val));
      }
      context.write(key, new ArrayFileIndexWritable(FileIndexWritable.class,
              fileList.toArray(new FileIndexWritable[fileList.size()])));
    }
  }

  public static void main(String[] args) throws Exception {
    Configuration conf = new Configuration();
    Job job = Job.getInstance(conf, "word count");
    job.setJarByClass(WordCount.class);
    job.setMapperClass(TokenizerMapper.class);
    //job.setCombinerClass(IntSumReducer.class);
    job.setReducerClass(IntSumReducer.class);
    job.setMapOutputKeyClass(Text.class);
    job.setMapOutputValueClass(FileIndexWritable.class);
    job.setOutputKeyClass(Text.class);
    job.setOutputValueClass(ArrayFileIndexWritable.class);
    FileInputFormat.addInputPath(job, new Path(args[0]));
    FileOutputFormat.setOutputPath(job, new Path(args[1]));
    System.exit(job.waitForCompletion(true) ? 0 : 1);
  }
}
