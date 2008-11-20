//
//  mathomaticViewController.h
//  mathomatic
//
//  Created by Ben Gotow on 10/23/08.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "KeyboardView.h"
#import "Operation.h"
#import "OperationIntegrate.h"
#import "OperationDerivative.h"
#import "OperationSolve.h"
#import "OperationLaplace.h"
#import "OperationFactor.h"
#import "OperationUnfactor.h"
#import "MathCommand.h"
#import "EquationView.h"

@interface MathomaticViewController : UIViewController <UITextFieldDelegate, UITableViewDataSource, UITableViewDelegate, UIActionSheetDelegate> {
    IBOutlet EquationView    *commandField;
    IBOutlet UITableView    *commandHistory;
    IBOutlet KeyboardView   *keyboard;
    
    NSMutableArray          *commandStack;
}

@property (nonatomic, retain) NSMutableArray * commandStack;

- (void)performString:(NSString*)entry;
- (void)performCommand:(MathCommand*)c;
- (void)addCommand:(MathCommand*)c;
- (void)keyboardEntryPerform:(NSString*)entry;
- (void)keyboardEntryComplete:(NSString*)entry;

@end

