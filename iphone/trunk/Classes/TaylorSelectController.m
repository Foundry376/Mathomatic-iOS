#import "TaylorSelectController.h"

@implementation TaylorSelectController

@synthesize operation;

- (void)viewDidLoad
{
    [pointField becomeFirstResponder];
}

- (void)setAvailableVariables:(NSArray*)variables
{
    [variable setEnabled:[variables containsObject:@"x"] forSegmentAtIndex:0];
    [variable setEnabled:[variables containsObject:@"y"] forSegmentAtIndex:1];
    [variable setEnabled:[variables containsObject:@"z"] forSegmentAtIndex:2];
}

- (IBAction)cancel 
{
    [self dismissViewControllerAnimated:YES completion:NULL];
}

- (IBAction)ok 
{
    NSString * message = nil;
    int order = [[orderField text] intValue];
    float point = [[pointField text] floatValue];
    
    if ([[orderField text] length] == 0)
        message = @"Please enter an order for the series, an integer indicating how many derivatives to take in the expansion.";
    if ([[pointField text] length] == 0)
        message = @"Please enter a value of the variable to expand around.";
    if (order <= 0)
        message = @"Please enter an order greater than 0.";
    
    if (message){
        UIAlertView * alert = [[UIAlertView alloc] initWithTitle:@"Error" message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [alert show];
        [alert release];
    } else {
        [operation parametersSelected: [[variable titleForSegmentAtIndex: [variable selectedSegmentIndex]] lowercaseString] point:point order:order];
        [self dismissViewControllerAnimated:YES completion:NULL];
    }
}
@end
