#import "AboutViewController.h"

@implementation AboutViewController

- (IBAction)close {
    [self dismissViewControllerAnimated:YES completion:NULL];
}

- (IBAction)showLink {
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"http://www.mathomatic.com"]];
}
@end
