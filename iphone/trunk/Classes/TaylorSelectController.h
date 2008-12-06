#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import "MathomaticOperationTaylor.h"

@interface TaylorSelectController : UIViewController {
    IBOutlet UIButton *okButton;
    IBOutlet UITextField *orderField;
    IBOutlet UITextField *pointField;
    IBOutlet UISegmentedControl *variable;
    MathomaticOperationTaylor * operation;
}

@property (nonatomic, retain) MathomaticOperationTaylor * operation;

- (void)setAvailableVariables:(NSArray*)variables;
- (IBAction)cancel;
- (IBAction)ok;
@end
