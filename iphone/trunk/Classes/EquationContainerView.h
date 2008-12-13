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
    
    BOOL                      openParen;
    BOOL                      closeParen;
    int                       minHeight;
}

@property (nonatomic, retain) NSMutableArray * innerEquations;
@property (nonatomic, retain) EquationContainerView * parent;

- (id)initWithParent:(EquationContainerView*)p;
- (EquationAbstractView*)popInnerEquation;
- (BOOL)addInnerEquation:(EquationAbstractView*)equation;
- (BOOL)innerEquationIsFull;
- (EquationAbstractView *)innerEquationAtOffset:(int)offset from:(EquationAbstractView*)v;
- (void)finalizeEquationTree:(BOOL)clean;
- (BOOL)finalizeEquationTreeForClass:(Class)c cleanParenthesis:(BOOL)clean;
- (void)finalizeTextSize:(int)parentTextSize;
- (void)finalizeFrame;
- (void)unlink;

- (void)setShowOpenParenthesis:(BOOL)showOpen;
- (void)setShowCloseParenthesis:(BOOL)showClose;
@end
