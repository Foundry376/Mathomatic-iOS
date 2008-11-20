//
//  EquationView.h
//  mathomatic
//
//  Created by Ben Gotow on 11/13/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface EquationView : UIScrollView {

    NSString        * equation;
    BOOL              editable;
    int               fontSize;
    int               equationHeight;
    
    UIView          * equationView;
}

@property (nonatomic, assign) BOOL editable;
@property (nonatomic, assign) int fontSize;
@property (nonatomic, assign) int equationHeight;

- (id)initWithEquation:(NSString*)e andFrame:(CGRect)frame andFontSize:(int)s;
- (void)setup;
- (void)assembleEquation;
- (void)setEquation:(NSString*)e;
- (NSString*)equation;

@end
