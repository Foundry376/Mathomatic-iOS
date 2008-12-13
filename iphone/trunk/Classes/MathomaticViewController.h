//
//  mathomaticViewController.h
//  mathomatic
//
//  Created by Ben Gotow on 10/23/08.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "KeyboardView.h"
#import "ExpressionButtonView.h"
#import "ExpressionScrollView.h"
#import "MathomaticExpression.h"
#import "MathomaticOperationTableViewCell.h"

@class MathomaticOperation;

@interface MathomaticViewController : UIViewController <UITextFieldDelegate, UITableViewDataSource, UITableViewDelegate, UIActionSheetDelegate, MathomaticOperationTableViewCellDelegate> {
    IBOutlet KeyboardView           *keyboard;
    IBOutlet UIButton               *keyboardSlideButton;
    IBOutlet UIActivityIndicatorView *spinner;
    NSTimer                         * spinnerTimer;
    IBOutlet UITableView            *commandHistory;
    NSMutableArray                  *commandStack;
    NSMutableArray                  *commandStackCells;
    
    BOOL                             keyboardVisible;
}

@property (nonatomic, retain) NSMutableArray * commandStack;

- (IBAction)aboutPressed:(id)sender;
- (IBAction)keyboardSlideToggle:(id)sender;
- (void)save;

- (void)performCommand:(MathomaticOperation*)c;
- (void)addCommand:(MathomaticOperation*)c;
- (BOOL)addKeyboardEntry;
- (MathomaticExpression*)lastExpression;
- (NSArray*)recentExpressions:(BOOL)equationsOnly unique:(BOOL)unique;

- (void)keyboardEntryPerform;
- (void)keyboardEntryComplete;

@end

