#import "AboutViewController.h"

@implementation AboutViewController

- (IBAction)close {
    [self dismissModalViewControllerAnimated: YES];
}

- (IBAction)showLink {
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"http://www.mathomatic.com"]];
}
@end
