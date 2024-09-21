# Project Archived

This project has been **archived**, and we encourage customers to upgrade to the new version (11.x) at [ISP Session Core](https://github.com/egbertn/ispsession.core).

the NUGet package is here [Nuget package ISP Session Core](https://www.nuget.org/packages/NCV.ISPSession)

![Archived](https://github.com/egbertn/ispsession.io/blob/master/archive-image-2.jpeg)

The new product is compatible with **.NET6** (and higher) and is currently designed to work with **Redis** and **Stack Exchange**.

Key Features:
- **Full encryption** and **compression**
- Minimal impact on the **.NET runtime**
- Supports Session State by either  **cookies**, **custom HTTP header**, **remote IP**(ip4 or ip6), **a form field** or even by **deep package inspection** of the HTTP request body.

This last option is ideal for integration with **webhooks**, where no browser is involved, but a 'session state' is still required.

This example project shows some ways to deal with sessions or application states or both.