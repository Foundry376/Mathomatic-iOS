//
//  EquationContainerView.h
//  mathomatic
//
//  Created by Ben Gotow on 11/14/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "EquationAbstractView.h"

@interface EquationContainerView : EquationAbstractView {

    NSMutableArray          * innerEquations;
    int                       innerEquationsMax;
    EquationContainerView   * parent;
    
    UILabel                 * openParen;
    UILabel                 * closeParen;
}

@property (nonatomic, retain) NSMutableArray * innerEquations;
@property (nonatomic, retain) EquationContainerView * parent;

- (id)initWithParent:(EquationContainerView*)p;
- (EquationAbstractView*)popInnerEquation;
- (BOOL)addInnerEquation:(EquationAbstractView*)equation;
- (BOOL)innerEquationIsFull;
- (void)finalizeEquationHierarchy;
- (void)finalizeTextSize:(int)parentTextSize;
- (void)finalizeFrame;

- (void)setShowOpenParenthesis:(BOOL)showOpen;
- (void)setShowCloseParenthesis:(BOOL)showClose;
@end
