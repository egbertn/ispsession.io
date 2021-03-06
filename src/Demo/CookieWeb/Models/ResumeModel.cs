﻿using System;
using System.ComponentModel.DataAnnotations;

namespace CookieWeb.Models
{
    public class ResumeModel
    {
        public ResumeModel()
        {
            Start = DateTime.UtcNow.ToBinary();
        }
        [Compare(nameof(MayPost), ErrorMessage = "You may not post")]
        public bool Check
        {
            get { return true; }
        }

     
        [Required, Range(1, long.MaxValue)]
        public long? Start { get; set; }
        public bool MayPost
        {
            get
            {
                return (DateTime.UtcNow - DateTime.FromBinary(Start?? DateTime.UtcNow.ToBinary())) > TimeSpan.FromSeconds(20);
            }
        }

        [MaxLength(32), Required, Display(Name ="Which favorite food")]
        public string Word { get; set; }
        [EmailAddress, Display(Name ="Your email address to which the URL is sent.")]
        public string Email { get; set; }
        public string SessionID { get; set; }
        public string Message { get; set; }
    }
}
