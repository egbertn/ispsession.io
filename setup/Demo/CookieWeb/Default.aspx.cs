using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
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
        Session["BigString"] = new string('0', 1000);
        if (Session["myintArray"] == null)
        {
            Session["myintArray"] = new[] { 1, 2, 3, 4 };
        }
        //if (Session["myobjects"] == null)
        //{
        //    var objects = new[] { new SomeData() { ONe = "Een", Two = 2, When = DateTime.UtcNow } };
        //    Session["myobjects"] = objects;
        //}

        // Session["realbig"] = new double[120000];
        var obj = new object[100, 100];
        for (int y = 0; y < 100; y++)
        {
            for (int x = 0; x < 100; x++)
            {
                obj[y, x] = new Random().Next(1000);
            }
        }
        obj[0, 0] = new object[] { 1, 2, 3, 4 };
        //warning, do not use Jagged arrays. ISP Session for classic ASP does not support it.
        //if (Session["jagged"] != null)
        //{
        //    txtJagged.Text =  ((int[][])Session["jagged"])[1][1].ToString();
        //}
        Session["BigArray"] = obj;
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