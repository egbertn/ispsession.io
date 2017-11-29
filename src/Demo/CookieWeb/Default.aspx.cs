using ispsession.io.Interfaces;
using System;
using System.Threading;
using System.Web.UI;
[Serializable]
public class SomeData
{
    public string ONe { get; set; }
    public int Two { get; set; }
    public DateTime? When { get; set; }
}


public partial class _Default : Page
{
    protected override void OnLoad(EventArgs e)
    {
        //not yet 
        var application = (IApplicationCache)this.Context.Items["_ISPApplicationCache"];
         var count = (int)(application["count"] ?? 0); 
        application["count"] = ++count;
        txtApplicationCache.Text = count.ToString();
        var countRefresh = Convert.ToInt32(Session["CountRefresh"] ?? 0); 
        countRefresh++;
        Session["CountRefresh"] = countRefresh;
        if (countRefresh > 10)
        {
            Session.Abandon();
        }

        if (IsPostBack)
        {
            if (Session["piet"] != null)
            {
                var arr = (object[, ,])Session["piet"];
                test.Text = (string)arr[1, 1, 1];
            }
        }
        else
        {
            var arr = new object[2, 2, 2];
            arr[1, 1, 1] = "hello";
            Session["piet"] = arr;
        }
        if (Session["a3"] != null)
        {
            var dt = ((object[,])Session["a3"])[0, 1];
        }
       // Session["Piet"] = "Hello";
        Session["randomize"] = new Random().Next();
      //  Session["BigString"] = new string('0', 1000);
        if (Session["myintArray"] == null)
        {
            Session["myintArray"] = new[] { 1, 2, 3, 4 };
        }
        //if (Session["myobjects"] == null)
        //{
        //    var objects = new[] { new SomeData() { ONe = "Een", Two = 2, When = DateTime.UtcNow } };
        //    Session["myobjects"] = objects;
        //}
        if (Session["realbig"] == null)
        {
            var arr  = new decimal[2000];
            arr[0] = 23403240432.234M;
            arr[1999] = 23403240432.234M;
            Session["realbig"] = arr;
        }
        else
        {
            realbig.Text = ((decimal[])Session["realbig"])[1999].ToString();
        }
        Thread.Sleep(new Random().Next(50, 500));
        //var obj = new object[100, 100];
        //for (int y = 0; y < 100; y++)
        //{
        //    for (int x = 0; x < 100; x++)
        //    {
        //        obj[y, x] = new Random().Next(1000);
        //    }
        //}
        //obj[0, 0] = new object[] { 1, 2, 3, 4 };
        //warning, do not use Jagged arrays. ISP Session for classic ASP does not support it.
        //if (Session["jagged"] != null)
        //{
        //    txtJagged.Text =  ((int[][])Session["jagged"])[1][1].ToString();
        //}
        //Session["BigArray"] = obj;
        //var jagged = new int[4][];
        //jagged[1] = new int[10];
        //jagged[0] = new int[10];
        //jagged[2] = new int[10];
        //jagged[3] = new int[10];
        //jagged[1][1]=123;
        //Session["jagged"] = jagged;
        //Session["arraywithLBound"] = arr;
       // Session["m"] = ' ';
    }
}