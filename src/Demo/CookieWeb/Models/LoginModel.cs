using System.ComponentModel.DataAnnotations;

namespace CookieWeb.Models
{
    public class LoginModel
    {
        [Compare("PasswordCheck", ErrorMessage = "INvalid Password/user combination"), MaxLength(10)]
        public string UserId { get; set; }
        [Compare("PasswordCheck", ErrorMessage = "INvalid Password/user combination"), MaxLength(10)]
        public string Password { get; set; }
        public string PasswordCheck { get { return "demo"; } set { } }
        public string ErrMsg { get; set; }
    }
}
