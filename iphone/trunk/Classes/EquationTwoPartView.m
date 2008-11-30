//
//  EquationDivisionView.m
//  mathomatic
//
//  Created by Ben Gotow on 11/14/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "EquationDivisionView.h"
#import "EquationTextView.h"

@implementation EquationTwoPartView

- (void)finalizeEquationTree:(BOOL)clean
{
    if ([innerEquations count] > 0)
        return;
        
    EquationAbstractView * next = [parent innerEquationAtOffset:1 from:self];
    EquationAbstractView * nextnext = [parent innerEquationAtOffset:2 from:self];
    EquationAbstractView * previous = [parent innerEquationAtOffset:-1 from:self];
    
    if (previous && next){
        [self addInnerEquation: previous];
        [parent.innerEquations removeObjectIdenticalTo: previous];
        
        if (([next isKindOfClass: [EquationTextView class]]) && ([[(EquationTextView*)next text] isEqualToString: @"-"]) && (nextnext)){
            EquationContainerView * container = [[[EquationContainerView alloc] initWithParent: self] autorelease];
            [container addInnerEquation: next];
            [container addInnerEquation: nextnext];
            [self addInnerEquation: container];
            
            [parent.innerEquations removeObjectIdenticalTo: next];
            [parent.innerEquations removeObjectIdenticalTo: nextnext];
        } else {
            [self addInnerEquation: next];
            [parent.innerEquations removeObjectIdenticalTo: next];
        }
    }
    
    for (int ii = [self.innerEquations count] - 1; ii >= 0; ii --)
        [[innerEquations objectAtIndex: ii] finalizeEquationTree: clean];
}

- (BOOL)isTreeLegal
{
    if ([innerEquations count] != 2)
        return NO;
        
    EquationAbstractView * a = [innerEquations objectAtIndex: 0];
    EquationAbstractView * b = [innerEquations objectAtIndex: 1];
    
    BOOL valid = NO;
    if ([a isKindOfClass: [EquationTextView class]])
        valid = ([(EquationTextView*)a isNumeric] || [(EquationTextView*)a isVariable]);
    else
        valid = [a isTreeLegal];
    
    if (!valid)
        return NO;
    
    if ([b isKindOfClass: [EquationTextView class]])
        valid = ([(EquationTextView*)b isNumeric] || [(EquationTextView*)b isVariable]);
    else
        valid = [b isTreeLegal];
    
    return valid;
}

- (void)dealloc {
    [super dealloc];
}


@end
